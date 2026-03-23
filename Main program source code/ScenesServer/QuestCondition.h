/**
 * \file
 * \version	$Id: QuestCondition.h  $
 * \author	
 * \date	
 * \brief	脚本触发条件
 * 
 */
 
#ifndef __QUEST_CONDITION_H__
#define __QUEST_CONDITION_H__

#include <string>

#include "Quest.h"
#include "QuestParse.h"

#include "Zebra.h"
#include "SceneUser.h"
#include "Scene.h"
#include "zObject.h"
#include "zDatabase.h"
#include "Command.h"

namespace Op {
	/**
	 * \brief 变量大小判定
	 *
	 * 判断一个变量是否大于给定值
	 *
	 */
	template <typename T>
	class Great
	{
	public:
		enum {
			NEED_EXIST = 1,
		};
		
		typedef T value_type;
		
		/**     
		 * \brief 判断给定变量是否大于测试值
		 *
		 * \param value: 待判断变量
		 * \param condition: 测试值
		 * \return 待判断变量大于测试值返回true,否则返回false
		 */     		
		bool operator() (T value, T condition) const
		{
			return (value>condition);
		}
		
		//added for debug
		std::string name() const 
		{
			return "Great";
		}
	};
	
	/**
	 * \brief 变量大小判定
	 *
	 * 判断一个变量是否小于给定值
	 *
	 */
	template <typename T>
	class Less
	{
	public:
		enum {
			NEED_EXIST = 0,
		};

		typedef T value_type;
		
		/**     
		 * \brief 判断给定变量是否小于测试值
		 *
		 * \param value: 待判断变量
		 * \param condition: 测试值
		 * \return 待判断变量小于测试值返回true,否则返回false
		 */     		
		bool operator() (T value, T condition) const
		{
			return (value<condition);
		}

		//added for debug
		std::string name() const 
		{
			return "Less";
		}
	};
	
	/**
	 * \brief 变量大小判定
	 *
	 * 判断一个变量是否等于给定值
	 *
	 */
	template <typename T>
	class Equal
	{
	public:
		enum {
			NEED_EXIST = 1,
		};

		typedef T value_type;
		
		/**     
		 * \brief 判断给定变量是否等于测试值
		 *
		 * \param value: 待判断变量
		 * \param condition: 测试值
		 * \return 待判断变量等于测试值返回true,否则返回false
		 */     		
		bool operator() (T value, T condition) const
		{
			return (value==condition);
		}

		//added for debug
		std::string name() const 
		{
			return "Equal";
		}
		
	};
	
	/**
	 * \brief 变量大小判定
	 *
	 * 判断一个变量不同于给定值
	 *
	 */
	template <typename T>
	class Differ
	{
	public:
		enum {
			NEED_EXIST = 0,
		};

		typedef T value_type;
		
		/**     
		 * \brief 判断给定变量是否不等于测试值
		 *
		 * \param value: 待判断变量
		 * \param condition: 测试值
		 * \return 待判断变量不等于测试值返回true,否则返回false
		 */     		
		bool operator() (T value, T condition) const
		{
			return (value!=condition);
		}

		//added for debug
		std::string name() const 
		{
			return "Differ";
		}
		
	};
}

/**
 * \brief 触发条件基类
 *
 * 该类定义了脚本触发条件的接口。
 *
 */	
class Condition
{
public:
	typedef Condition Base;

	virtual bool is_valid (const SceneUser* user, const Vars* vars) const;

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~Condition() { }
	
protected:
	virtual bool check_args(const SceneUser* user, const Vars* vars) const;
	
	virtual bool check_valid(const SceneUser* user, const Vars* vars) const = 0;
};

/**
 * \brief 组队触发条件基类
 *
 * 该类提供了对于组队任务的支持
 *
 */	
class TeamCondition : public Condition
{
public:
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	TeamCondition(Parse& p)
	{ 
		p.value("team", _team);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~TeamCondition() { }
	
	virtual bool is_valid(const SceneUser* user, const Vars* vars) const;
	
protected:	
	
private:
	int _team;
};

/**
 * \brief 变量条件
 *
 * 该类提供了对于任务脚本中有关变量的条件判定的支持
 *
 */
template <typename Operation>
class VarCondition : public TeamCondition
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
	VarCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("name", _name);
		p.value("value", _condition);
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~VarCondition()
	{ }

	
	/**     
	 * \brief  变量判定
	 *
	 * 重载了check_valid函数,判定某个变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
//		Zebra::logger->debug("id:%d\tname:%s\tcondition:%d\toperation:%s", _id, _name.c_str(), _condition, op.name().c_str());
		if (_id) {
			Vars* v = user->quest_list.vars(_id);
			if (v) {
				return v->is_valid(op, _name, _condition);
			}else {
				return !Operation::NEED_EXIST;
			}
		}
			
		return vars->is_valid(op, _name, _condition);
	};

private:
	std::string _name;
	value_type _condition;
	int _id;
};

/**
 * \brief 用户变量条件
 *
 * 该类提供了对于任务脚本中有关用户变量的条件判定的支持
 *
 */
template <typename Operation>
class UserVarCondition : public TeamCondition
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
	UserVarCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("name", _name);
		p.value("value", _condition);
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~UserVarCondition()
	{ }

	/**     
	 * \brief  用户变量判定
	 *
	 * 重载了check_valid函数,判定某个用户变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		int id = _id?_id:vars->quest_id();

		QWORD key = ((QWORD)user->charbase.accid << 32) | user->charbase.id;
		Vars* v = UserVar::instance().vars(id, key);
		if (v) {
			return v->is_valid(op, _name, _condition);
		}

		return !Operation::NEED_EXIST;
		
	};

private:
	std::string _name;
	value_type _condition;
	int _id;
};

/**
 * \brief 全局变量条件
 *
 * 该类提供了对于任务脚本中有关全局变量的条件判定的支持
 *
 */
template <typename Operation>
class GlobalCondition : public Condition
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
	GlobalCondition(Parse& p)
	{ 
		p.value("name", _name);
		p.value("value", _condition);
		p.value("id", _id);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~GlobalCondition()
	{ }

	/**     
	 * \brief  参数检查
	 *
	 * 重载了check_args函数,该条件不需要用户信息
	 *      
	 * \param user: NULL
	 * \param vars: 变量信息
	 * \return 变量有效返回true,否则返回false
	 */ 	
	bool check_args(const SceneUser* user, const Vars* vars) const
	{
		if (!vars) return false;
		
		return true;
	}

	/**     
	 * \brief  全局变量判定
	 *
	 * 重载了check_valid函数,判定某个全局变量是否满足脚本要求
	 *      
	 * \param user: NULL
	 * \param vars: 变量信息
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		int id = _id?_id:vars->quest_id();

//		Zebra::logger->debug("id:%d\tname:%s\tcondition:%d\toperation:%s", id, _name.c_str(), _condition, op.name().c_str());
		
		Vars* v = GlobalVar::instance().vars(id);
		if (v) {
			return v->is_valid(op, _name, _condition);
		}
		
		return !Operation::NEED_EXIST;
	};

protected:
	std::string _name;
	value_type _condition;
	int _id;
};

template <typename Operation>
class TongVarCondition : public GlobalCondition<Operation>
{
public:
	TongVarCondition(Parse& p) : GlobalCondition<Operation>(p)
	{ 
	}

	virtual ~TongVarCondition()
	{ }

	bool check_args(const SceneUser* user, const Vars* vars) const
	{
		if (!user) return false;
		
		return true;
	}

	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;

		Vars* v = TongVar::instance().vars(user->charbase.unionid);
		if (v) {
			return v->is_valid(op, GlobalCondition<Operation>::_name, GlobalCondition<Operation>::_condition);
		}
		
		return !Operation::NEED_EXIST;
	};
};

template <typename Operation>
class FamilyVarCondition : public TongVarCondition<Operation>
{
public:
	FamilyVarCondition(Parse& p) : TongVarCondition<Operation>(p)
	{ 
	}

	virtual ~FamilyVarCondition()
	{ }

	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;

		Vars* v = FamilyVar::instance().vars(user->charbase.septid);
		if (v) {
			return v->is_valid(op, GlobalCondition<Operation>::_name, GlobalCondition<Operation>::_condition);
		}
		
		return !Operation::NEED_EXIST;
	};
};

template <typename Operation>
class UsersVarCondition : public TongVarCondition<Operation>
{
public:
	UsersVarCondition(Parse& p) : TongVarCondition<Operation>(p)
	{ 
	}

	virtual ~UsersVarCondition()
	{ }

	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;

		Vars* v = UsersVar::instance().vars( ((QWORD)user->charbase.accid << 32) | user->charbase.id);
		if (v) {
			return v->is_valid(op, GlobalCondition<Operation>::_name, GlobalCondition<Operation>::_condition);
		}
		
		return !Operation::NEED_EXIST;
	};
};

/**
 * \brief 技能是否学习条件
 *
 * 该类提供了判断用户是否学习技能
 *
 */
template <typename Operation>
class HaveSkillCondition : public TeamCondition
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
	HaveSkillCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~HaveSkillCondition()
	{ }
	
	/**     
	 * \brief  技能个数判定
	 *
	 * 重载了check_valid函数,判定用户的技能数量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		int size=user->usm.size();
		return op(size, _condition);
	}

private:
	value_type _condition;		
};


/**
 * \brief 检查玩家性别
 *
 * 该类提供了判断用户的性别
 *
 */
template <typename Operation>
class CheckSexCondition : public TeamCondition
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
	CheckSexCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CheckSexCondition()
	{ }
	
	/**     
	 * \brief  技能个数判定
	 *
	 * 重载了check_valid函数,判定用户的技能数量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		int sex = 0;
		switch(user->charbase.type)
		{
			case PROFESSION_1:		//侠客
			case PROFESSION_3:		//箭侠
			case PROFESSION_5:		//天师
			case PROFESSION_7:		//法师
				sex = 1;
				break;
			case PROFESSION_2:		//侠女
			case PROFESSION_4:		//箭灵
			case PROFESSION_6:		//美女
			case PROFESSION_8:		//仙女
				sex = 0;
				break;
			case PROFESSION_NONE:	//无业
			default:
				break;
		}
		return op(sex, _condition);
	}

private:
	value_type _condition;		
};

/**
 * \brief 等级条件
 *
 * 该类提供了判断用户等级是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class LevelCondition : public TeamCondition
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
	LevelCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~LevelCondition()
	{ }
	
	/**     
	 * \brief  等级判定
	 *
	 * 重载了check_valid函数,判定用户的等级是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.level, _condition);
	}

private:
	value_type _condition;		
};


/**
 * \brief 家族条件
 *
 * 该类提供了判断用户家族ID是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class SeptIDCondition : public TeamCondition
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
	SeptIDCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~SeptIDCondition()
	{ }
	
	/**     
	 * \brief  家族判定
	 *
	 * 重载了check_valid函数,判定用户的家族ID是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.septid, _condition);
	}

private:
	value_type _condition;		
};

/**
 * \brief 帮会条件
 *
 * 该类提供了判断用户家族ID是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class UnionIDCondition : public TeamCondition
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
	UnionIDCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~UnionIDCondition()
	{ }
	
	/**     
	 * \brief  帮会判定
	 *
	 * 重载了check_valid函数,判定用户的家族ID是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.unionid, _condition);
	}

private:
	value_type _condition;		
};

#include "Chat.h"

/**
 * \brief 金钱条件
 *
 * 该类提供了判断用户包裹中金钱数量是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class GoldCondition : public TeamCondition
{
public:	
	/**     
	 * \brief  构造函数
	 *
	 * 初始化相关变量
	 *      
	 * \param p: 关键词列表
	 */     	
	GoldCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~GoldCondition()
	{ }

	/**     
	 * \brief  金钱判定
	 *
	 * 重载了check_valid函数,判定用户包裹中的金钱是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		zObject* gold = const_cast<SceneUser *>(user)->packs.getGold();
		
		DWORD number = 0;
		if (gold)  number = gold->data.dwNum;
		Operation op;

		//shouldn't be exist, NB CEHUA
		//if (op.name() == "Great" && number < (DWORD)_condition) {
			//Channel::sendSys(const_cast<SceneUser*>(user), Cmd::INFO_TYPE_FAIL, "金钱不足");
		//}
		
		return op(number, _condition);
	}

private:
	int _condition;		
};

////////////////////////////////////////////////////////////
//soke 增加金子条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 金子条件
 *
 * 该类提供了判断用户等级是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class ZiseGoldCondition : public TeamCondition
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
	ZiseGoldCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ZiseGoldCondition()
	{ }
	
	/**     
	 * \brief  金子判定
	 *
	 * 重载了check_valid函数,判定用户的金子是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.gold, _condition);
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////////
//soke 增加积分点数条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 积分条件
 *
 * 该类提供了判断用户等级是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class TicketCondition : public TeamCondition
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
	TicketCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~TicketCondition()
	{ }
	
	/**     
	 * \brief  积分判定
	 *
	 * 重载了check_valid函数,判定用户的积分是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.ticket, _condition);
	}

private:
	value_type _condition;		
};


////////////////////////////////////////////////////////////
//soke 增加工资条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 工资条件
 *
 * 该类提供了判断用户等级是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class SalaryCondition : public TeamCondition
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
	SalaryCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~SalaryCondition()
	{ }
	
	/**     
	 * \brief  工资判定
	 *
	 * 重载了check_valid函数,判定用户的工资是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.salary, _condition);
	}

private:
	value_type _condition;		
};



////////////////////////////////////////////////////////////
//MYY 增加战力值判断点数条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 战力点数条件
 *
 * 该类提供了判断用户战力点数是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class CharzhanliCondition : public TeamCondition
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
	CharzhanliCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CharzhanliCondition()
	{ }
	
	/**     
	 * \brief  战力值点数判定
	 *
	 * 重载了check_valid函数,判定用户的战力值点数是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.zhanli, _condition);
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////////
//soke 增加VIP点数条件判断
////////////////////////////////////////////////////////////
/**
 * \brief VIP点数条件
 *
 * 该类提供了判断用户VIP点数是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class CharvipCondition : public TeamCondition
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
	CharvipCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CharvipCondition()
	{ }
	
	/**     
	 * \brief  VIP点数判定
	 *
	 * 重载了check_valid函数,判定用户的VIP点数是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.charvip, _condition);
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////////
//soke 增加充值点数条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 充值点数条件
 *
 * 该类提供了判断用户充值点数是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class ChongzhiCondition : public TeamCondition
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
	ChongzhiCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ChongzhiCondition()
	{ }
	
	/**     
	 * \brief  充值点数判定
	 *
	 * 重载了check_valid函数,判定用户的充值点数是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.chongzhi, _condition);
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////////
//soke 增加职业点数条件判断
////////////////////////////////////////////////////////////
/**
 * \brief 职业点数条件
 *
 * 该类提供了判断用户职业点数是否大于,小于,等于或不等于某个值的接口
 *
 */
template <typename Operation>
class ChangejobCondition : public TeamCondition
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
	ChangejobCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ChangejobCondition()
	{ }
	
	/**     
	 * \brief  职业点数判定
	 *
	 * 重载了check_valid函数,判定用户的职业点数是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		Operation op;
		return op(user->charbase.changejob, _condition);
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////////////

/**
 * \brief 物品条件
 *
 * 该类提供了判断用户包裹中的特定物品数量是否大于,小于,等于或不等于某个值的接口
 *
 */
class ItemCondition : public TeamCondition
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
	ItemCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("id", _id);
		p.value("value", _condition);
		p.value("level", _level);
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ItemCondition()
	{ }

	/**     
	 * \brief  物品数量判定
	 *
	 * 重载了check_valid函数,判定用户包裹空间的某个物品数量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return user->packs.uom.exist(_id, _condition, _level);
	}

private:
	int _id;
	int _level;
	value_type _condition;		
};

//soke 判断是否在某张地图某坐标

class MapPosCondition : public TeamCondition
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
	MapPosCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
		p.value("mapid", _MapID);	
		std::string pos;		
		p.value("posx", _PosX);	
		p.value("posy", _PosY);	
	}

	/**     
	* \brief 析构函数
	*
	*/    
	virtual ~MapPosCondition()
	{ }

	bool  check_valid(const SceneUser* user, const Vars* vars) const
	{
		if (user->scene->getRealMapID()!=_MapID)
		{
			if (_condition>0)
			{	
				Channel::sendSys(const_cast<SceneUser*>(user), Cmd::INFO_TYPE_EXP, "宝物不在你当前所在的地图");
			}
			return false;
		}
		if (abs(user->getPos().x-_PosX)<=2&&abs(user->getPos().y-_PosY)<=2)
		{
			return true;
		}
		if(_condition>0)
		{

			char s[256];
			int  n=0;
			if (abs(user->getPos().x-_PosX)==abs(user->getPos().y-_PosY))//斜向
			{
				if (user->getPos().y<_PosY)//南
				{
					if (user->getPos().x<_PosX)//东
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的右下方%d步",n);
						//sprintf(s,"宝物在你的东南方%d步",n);
					}
					if (user->getPos().x>_PosX)//西
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的左下方%d步",n);
						//sprintf(s,"宝物在你的西南方%d步",n);
					}
				}
				if (user->getPos().y>_PosY)//北
				{
					if (user->getPos().x<_PosX)//东
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的右上方%d步",n);
						//sprintf(s,"宝物在你的东北方%d步",n)
					}
					if (user->getPos().x>_PosX)//西
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的左上方%d步",n);
						//sprintf(s,"宝物在你的西北方%d步",n);
					}
				}
			}
			else//直向
			{
				if (abs(user->getPos().x-_PosX)>abs(user->getPos().y-_PosY))//南北
				{

					if (user->getPos().x<_PosX)
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的右方%d步",n);
						//sprintf(s,"宝物在你的东方%d步",n);
					}
					if (user->getPos().x>_PosX)
					{
						n=abs(user->getPos().x-_PosX)-2;
						sprintf(s,"宝物在你的左方%d步",n);
						//sprintf(s,"宝物在你的西方%d步",n);
					}
				}
				else//东西
				{
					if (user->getPos().y<_PosY)
					{
						n=abs(user->getPos().y-_PosY)-2;
						sprintf(s,"宝物在你的下方%d步",n);
						//sprintf(s,"宝物在你的南方%d步",n);
					}
					if (user->getPos().y>_PosY)
					{
						n=abs(user->getPos().y-_PosY)-2;
						sprintf(s,"宝物在你的上方%d步",n);
						//sprintf(s,"宝物在你的北方%d步",n);
					}
				}
			}

			Channel::sendSys(const_cast<SceneUser*>(user), Cmd::INFO_TYPE_EXP, s);
		}
		return false;

	}


private:
	value_type _condition;	
	DWORD  _MapID;
	DWORD _PosX;
	DWORD _PosY;
};

//////////////////////////////////////////////////////////////////////////

//soke 判断任务ID和地图ID是否符合条件

class MapidCondition : public TeamCondition
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
	MapidCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("id", _id);	
		p.value("name", _Name);	
	}

	/**     
	* \brief 析构函数
	*
	*/    
	virtual ~MapidCondition()
	{ }

	bool  check_valid(const SceneUser* user, const Vars* vars) const
	{
		if (user->scene->getRealMapID()!=_Name)
		{
			if (_id)
			{	
				user->quest_list.vars(_id);
			}
			return false;
		}
		if (user->scene->getRealMapID()==_Name)
		{
			if (_id)
			{	
				user->quest_list.vars(_id);
			}
			return true;
		}
		return false;
	}

private:
	int _id;	
	DWORD  _Name;
};

//////////////////////////////////////////////////////////////////////////

/**
 * \brief 国家条件
 *
 * 该类提供了判断用户是否属于一个国家的接口
 *
 */
class NationCondition : public TeamCondition
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
	NationCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~NationCondition()
	{ }
	
	/**     
	 * \brief  国家判定
	 *
	 * 重载了check_valid函数,判定用户是否属于某个国家
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)user->charbase.country == _condition;
	}

private:
	value_type _condition;		
};

////////////////////////////////////////////////////////
/**
 * \brief 国家条件
 *
 * 该类提供了判断用户是否属于一个国家的接口
 *
 */
//soke 新的支持国家判断
class CountryidCondition : public TeamCondition
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
	CountryidCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CountryidCondition()
	{ }
	
	/**     
	 * \brief  国家判定
	 *
	 * 重载了check_valid函数,判定用户是否属于某个国家
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)user->charbase.country == _condition;
	}

private:
	value_type _condition;		
};

/**
 * \brief 职业条件
 *
 * 该类提供了判断用户是否属于一个职业的接口
 *
 */
//soke 新的支持职业判断
class ChangeCondition : public TeamCondition
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
	ChangeCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ChangeCondition()
	{ }
	
	/**     
	 * \brief  职业判定
	 *
	 * 重载了check_valid函数,判定用户是否属于某个职业
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)user->charbase.changejob == _condition;
	}

private:
	value_type _condition;		
};

/**
 * \brief 性别条件
 *
 * 该类提供了判断用户是否属于男性或女性的接口
 *
 */
//soke 新的支持性别判断
class SexCondition : public TeamCondition
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
	SexCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~SexCondition()
	{ }
	
	/**     
	 * \brief 性别判定
	 *
	 * 重载了check_valid函数,判定用户是否属于男性或女性性别
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)user->charbase.type == _condition;
	}

private:
	value_type _condition;		
};
//////////////////////////////////////////////////////////

/**
 * \brief 是否在本国条件
 *
 * 该类提供了判断用户是否在本国的接口
 *
 */
class InNationCondition : public TeamCondition
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
	InNationCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~InNationCondition()
	{ }
	
	/**     
	 * \brief  国家判定
	 *
	 * 重载了check_valid函数,判定用户是否属于某个国家
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return user->charbase.country == user->scene->getCountryID();
	}

private:
	value_type _condition;		
};

/**
 * \brief 帮会条件
 *
 * 该类提供了判断用户是否属于某个帮会的接口
 *
 */
class ConfraternityCondition : public TeamCondition
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
	ConfraternityCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ConfraternityCondition()
	{ }
	
	/**     
	 * \brief  帮会判定
	 *
	 * 重载了check_valid函数,判定用户是否属于某个帮会
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)(user->charbase.unionid & 0x1) == _condition;
	}

private:
	value_type _condition;		
};

/**
 * \brief 职业条件
 *
 * 该类提供了判断用户是否属于某个职业的接口
 *
 */
class ProfessionCondition : public TeamCondition
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
	ProfessionCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _condition);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ProfessionCondition()
	{ }
	
	/**     
	 * \brief  职业判定
	 *
	 * 重载了check_valid函数,判定用户职业是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return user->charbase.type & _condition;
	}

private:
	value_type _condition;		
};

/**
 * \brief 包裹空间条件
 *
 * 该类提供了判断用户包裹中的空间是否能容纳某个物品的接口
 *
 */
class SpaceCondition : public TeamCondition
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
	SpaceCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("size", _size);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~SpaceCondition()
	{ }
	
	/**     
	 * \brief  包裹空间判定
	 *
	 * 重载了check_valid函数,判定用户包裹空间是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		int free = user->packs.uom.space(user);

		if ( free >= _size) 	return true;

		//Channel::sendSys(const_cast<SceneUser*>(user), Cmd::INFO_TYPE_FAIL, "包裹空间不足");
		return false;
	}

private:
	value_type _size;		
};

/**
 * \brief 是否超过任务时间要求
 *
 * 该类提供了判断任务时间是否满足要求的接口
 *
 */
class TimeoutsCondition : public TeamCondition
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
	TimeoutsCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("long", _time);	
		p.value("id", _id);	
		p.value("less", _less);
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~TimeoutsCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定任务时间是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		if (_id) 
		{
			Vars* v = user->quest_list.vars(_id);
			if (v) 
			{
				return _less?(v->is_timeout(_time)):(!v->is_timeout(_time));
			}
			return true;
		}
			
		return _less?(vars->is_timeout(_time)):(!vars->is_timeout(_time));
	}

private:
	value_type _time;		
	int _id;
	int _less;
};

/**
 * \brief 时间条件
 *
 * 该类提供了判断系统时间是否满足要求的接口
 *
 */
class TimerCondition : public Condition
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
	TimerCondition(Parse& p)
	{ 
		std::string start;
		p.value("start", start);
		strptime(start.c_str(), "%H:%M:%S", &_start);		
		std::string end;
		p.value("end", end);
		strptime(end.c_str(), "%H:%M:%S", &_end);				
	}


	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~TimerCondition()
	{ }

	/**     
	 * \brief  参数检查
	 *
	 * 重载了check_args函数,该条件不需要用户及变量信息
	 *      
	 * \param user: NULL
	 * \param vars: NULL
	 * \return 当前总是返回true
	 */ 	
	bool check_args(const SceneUser* user, const Vars* vars) const
	{
		return true;
	}

	/**     
	 * \brief  时间条件判定
	 *
	 * 重载了check_valid函数,判定系统时间是否满足脚本要求
	 *      
	 * \param user: NULL
	 * \param vars: NULL
	 * \return true表示满足条件,false表示不满足条件
	 */   	
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
        	struct tm now;
        	time_t timValue = time(NULL);
        	zRTime::getLocalTime(now, timValue);
		//Zebra::logger->debug("Time_START:%02d:%02d:%02d", _start.tm_hour, _start.tm_min, _start.tm_sec);
		//Zebra::logger->debug("Time_NOW  :%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
		//Zebra::logger->debug("Time_END  :%02d:%02d:%02d", _end.tm_hour, _end.tm_min, _end.tm_sec);
		if ( ((now.tm_hour > _start.tm_hour) || ( (now.tm_hour == _start.tm_hour) && (now.tm_min > _start.tm_min) ) 
			|| ((now.tm_hour == _start.tm_hour) && (now.tm_min == _start.tm_min) && (now.tm_sec >= _start.tm_sec))) //start
			&& ((now.tm_hour < _end.tm_hour) || ( (now.tm_hour == _end.tm_hour) && (now.tm_min < _end.tm_min) ) 
			|| ((now.tm_hour == _end.tm_hour) && (now.tm_min == _end.tm_min) && (now.tm_sec < _end.tm_sec))) 	 //end
			) {
			return true;
		}	

		return false;	
	}

private:
	struct tm _start;
	struct tm _end;	
};

/**
 * \brief 组队条件
 *
 * 该类提供了判断用户队伍是否满足要求的接口
 *
 */
class TeamedCondition : public Condition
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
	TeamedCondition(Parse& p) 
	{ 
		p.value("number", _number);	
		p.value("male", _male);
		p.value("female", _female);
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~TeamedCondition()
	{ }
	
	bool check_valid(const SceneUser* user, const Vars* vars) const;

private:
	value_type _number;	
	value_type _male;
	value_type _female;
};

class IsGuardCondition : public TeamCondition
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
	IsGuardCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _need);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~IsGuardCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定任务时间是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		int guard = user->guard?1:0;
		return guard == _need;
	}
private:
	value_type _need;
};

class FiveTypeCondition : public TeamCondition
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
	FiveTypeCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _need);	
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~FiveTypeCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定任务时间是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return (int)user->charbase.fivetype == _need;
	}
private:
	value_type _need;
};

template <typename Operation>
class FiveLevelCondition : public TeamCondition
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
	FiveLevelCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _level);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~FiveLevelCondition()
	{ }

	
	/**     
	 * \brief  变量判定
	 *
	 * 重载了check_valid函数,判定某个变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
			
		return op((int)user->charbase.fivelevel, _level);
	};

private:
	value_type _level;
};

template <typename Operation>
class FamilyLevelCondition : public TeamCondition
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
	FamilyLevelCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _level);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~FamilyLevelCondition() { }

	
	/**     
	 * \brief  变量判定
	 *
	 * 重载了check_valid函数,判定某个变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
			
		return op((int)user->dwSeptLevel, _level);
	};

private:
	value_type _level;
};

template <typename Operation>
class ReputeCondition : public TeamCondition
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
	ReputeCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _level);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ReputeCondition()
	{ }

	
	/**     
	 * \brief  变量判定
	 *
	 * 重载了check_valid函数,判定某个变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
			
		return op((int)user->dwSeptRepute, _level);
	};

private:
	value_type _level;
};

template <typename Operation>
class ActionPointCondition : public TeamCondition
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
	ActionPointCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _level);
	}
	
	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~ActionPointCondition()
	{ }

	
	/**     
	 * \brief  变量判定
	 *
	 * 重载了check_valid函数,判定某个变量是否满足脚本要求
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid (const SceneUser* user, const Vars* vars) const
	{
		Operation op;
			
		return op((int)user->dwUnionActionPoint, _level);
	};

private:
	value_type _level;
};

class HorseCondition : public TeamCondition
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
	HorseCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _id);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~HorseCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		return user->horse.horse() == (DWORD)_id;
	}
private:
	value_type _id;
};

class GradeCondition : public TeamCondition
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
	GradeCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _grade);	
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~GradeCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const
	{
		if (_grade == 1) return user->king;
		if (_grade == 2) return user->unionMaster;
		if (_grade == 4) return user->septMaster;
		
		return true;
	}
private:
	value_type _grade;
};

class MapCountryCondition : public TeamCondition
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
	MapCountryCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("name", _name);
		p.value("id", _id);
	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~MapCountryCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	std::string _name;
	int _id;
};

class HonorCondition : public TeamCondition
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
	HonorCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~HonorCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};

class MaxHonorCondition : public TeamCondition
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
	MaxHonorCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~MaxHonorCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};

class SelfCountryCondition : public TeamCondition
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
	SelfCountryCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~SelfCountryCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定用户是否拥有所需马匹
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};

class CountryPowerCondition : public TeamCondition
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
	CountryPowerCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CountryPowerCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定国家强弱
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};


class WeekCondition : public TeamCondition
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
	WeekCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~WeekCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定今天是否是一星期中指定的那几天 value值中用0-6位表示一周的7天
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};


class CaptionCondition : public TeamCondition
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
	CaptionCondition(Parse& p) : TeamCondition(p)
	{ 
		p.value("value", _value);
 	}

	/**     
	 * \brief 析构函数
	 *
	 */    
	virtual ~CaptionCondition()
	{ }
	
	/**     
	 * \brief  任务时间判定
	 *
	 * 重载了check_valid函数,判定角色是否国王或者城主
	 *      
	 * \param user: 触发条件的用户
	 * \param vars: 用户所带的该任务相关变量
	 * \return true表示满足条件,false表示不满足条件
	 */    
	bool check_valid(const SceneUser* user, const Vars* vars) const;
private:
	int _value;
};

#endif

