/**
 * \file
 * \version	$Id: QuestMaker.h  $
 * \author	
 * \date		2005-04-07
 * \brief		产品创建工厂
 * 
 */
 
#ifndef __QUEST_MAKER_H__
#define __QUEST_MAKER_H__

/**
 * \brief 产品工厂
 *
 * 封装了工厂模式
 *
 */
template <typename I, typename C>
class ProductFactory
{
public:
	typedef ProductFactory<I, C> self_t;
	
	/**     
	 * \brief 
	 *
	 * 实现了单件模式
	 *      
	 * \return 该类的唯一实例
	 */     	
	static self_t& instance()
	{
		if (!_instance) {
			static self_t new_instance;
			_instance = &new_instance;
		}
		
		return *_instance;
	}

	/**     
	 * \brief 取得创建类
	 *
	 * 寻找特定产品标志的创建类
	 *      
	 * \param id: 产品标识
	 * \return 找的产品创建类,没找到返回NULL
	 */     	
	C* get_creator(const I& id) const
	{		
		const_list_iterator it = _list.find(id);
		if ( it!= _list.end() ) {
			return it->second;
		}
		
		return NULL;		
	}
	
	/**     
	 * \brief 注册方法
	 *
	 * 注册某类产品的创建类
	 *      
	 * \param id: 产品标识
	 * \param creator: 产品创建类
	 * \return 无
	 */     	
	void register_creator(const I& id, C* creator)	
	{
		_list[id] = creator;
	}
private:
	/**     
	 * \brief 构造函数
	 *
	 */     
	ProductFactory()
	{ }
	
	/**     
	 * \brief 析构函数
	 *
	 */     
	~ProductFactory()
	{ }
	
	static self_t* _instance;
	
	typedef std::map<I, C*> LIST;
	typedef typename LIST::iterator list_iterator;
	typedef typename LIST::const_iterator const_list_iterator;
	LIST _list;
};

template<typename I, typename C>
ProductFactory<I, C>* ProductFactory<I, C>::_instance = NULL;

#include "QuestParse.h"
#include "QuestCondition.h"
#include "QuestAction.h"
#include "zXMLParser.h"

namespace MakeFunction {
	/**
	 * \brief 产品创建基类
	 *
	 *定义了创建产品的接口
	 *
	 */
	template <typename A>
	class Maker
	{
	public:	
		typedef typename A::Base Base;

		/**     
		 * \brief  产品创建接口
		 *
		 * 定义了产品创建 的接口,继承类需要实现此函数提供具体产品的创建方法.
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */     	
		virtual Base* make ( zXMLParser& xml, xmlNodePtr& node) = 0;

		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Maker() { }
	};
	
	/**
	 * \brief 节点内容创建类
	 *
	 *封装了需要访问节点内容的脚本触发条件及动作类的创建
	 *
	 */	
	template <typename A>
	class Content : public Maker<typename A::Base>
	{
	public:	
		typedef typename A::Base Base;
		
		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Content() { }
		
		/**     
		 * \brief  产品创建接口
		 *
		 *实现对访问节点内容的脚本触发条件及动作类的创建
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */     	
		Base* make (zXMLParser& xml, xmlNodePtr& node)
		{
			std::string content;
			if (!xml.getNodeContentStr(node, content)) {
				return NULL;
			}
			
			return new A(content);			
		}
	};
	
	/**
	 * \brief 空属性创建类
	 *
	 *封装了不需要访问节点属性的脚本触发条件及动作类的创建
	 *
	 */	
	template <typename A>
	class Void : public Maker<Action>
	{
	public:	
		typedef Maker<Action>::Base Base;
		
		/**     
		 * \brief  构造函数
		 *
		 * 初始化相关变量
		 *      
		 */     	
		Void()
		{ }
		
		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Void() { }
		
		/**     
		 * \brief  产品创建接口
		 *
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */     	
		Base* make (zXMLParser& xml, xmlNodePtr& node)
		{
			return  new A();
		}
	};

	/**
	 * \brief 节点属性创建类
	 *
	 *封装了需要访问节点属性的脚本触发条件及动作类的创建
	 *
	 */	
	template <typename A>
	class Prop : public Maker<typename A::Base>
	{
	public:	
		typedef typename A::Base Base;
		
		/**     
		 * \brief  构造函数
		 *
		 * 初始化相关变量
		 *      
		 * \param p: 关键词列表
		 */     	
		Prop(Parse& p) : _p(p)
		{ }
		
		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Prop() { }
		
		/**     
		 * \brief  产品创建接口
		 *
		 *实现对访问节点属性的脚本触发条件及动作类的创建
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */     	
		Base* make (zXMLParser& xml, xmlNodePtr& node)
		{
			if (! _p.parse(xml, node) ) return NULL;

			return new A(_p);			
		}
	private:
	
		Parse _p;
	};

	/**
	 * \brief 触发动作创建类
	 *
	 *封装了需要按操作类型更新的脚本触发动作类的创建
	 *
	 */	
	template < template <typename> class A, typename T = int >
	class Update : public Maker<Action>
	{
	public:	
		typedef Maker<Action>::Base Base;
		
		/**     
		 * \brief  构造函数
		 *
		 * 初始化相关变量
		 *      
		 * \param p: 关键词列表
		 */     	
		Update(Parse& p) : _p(p)
		{
			
		}
		
		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Update() { }
		
		/**     
		 * \brief  产品创建接口
		 *
		 *实现对按操作类型更新的脚本触发动作类的创建
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */     	
		Base* make (zXMLParser& xml, xmlNodePtr& node)
		{
			if (! _p.parse(xml, node) ) return NULL;

			std::string type;
			_p.value("type", type);

			if (!type.compare("Set")) {
				return new A< Op::Set<T> >(_p);	
			}else if (!type.compare("Add")) {
				return new A< Op::Add<T> >(_p);											
			}else if (!type.compare("Sub")) {
				return new A< Op::Sub<T> >(_p);									
			}else if (!type.compare("Mul")) {
				return new A< Op::Mul<T> >(_p);									
			}else if (!type.compare("Pow")) {
				return new A< Op::Pow<T> >(_p);									
			}else if (!type.compare("GetP")) {
				return new A< Op::GetP<T> >(_p);									
			}else if (!type.compare("Div")) {
				return new A< Op::Div<T> >(_p);									
			}

			return NULL;
		}
		
		private:
			Parse _p;
	};

	/**
	 * \brief 触发条件创建类
	 *
	 *封装了需要按操作类型比较的脚本触发条件类的创建
	 *
	 */	
	template <template <typename> class A, typename T = int >
	class Compare : public Maker<Condition>
	{
	public:	
		typedef Maker<Condition>::Base Base;
		
		/**     
		 * \brief  构造函数
		 *
		 * 初始化相关变量
		 *      
		 * \param p: 关键词列表
		 */     	
		Compare(Parse& p) : _p(p)
		{ 
			
		}
		
		/**     
		 * \brief 析构函数
		 *
		 */     
		virtual ~Compare() { }
		
		/**     
		 * \brief  产品创建接口
		 *
		 *实现对按操作类型比较的脚本触发条件类的创建
		 *      
		 * \param xml: 脚本文件
		 * \param node: 节点名称
		 * \return 创建的产品
		 */ 
		Base* make (zXMLParser& xml, xmlNodePtr& node)
		{
			if (! _p.parse(xml, node) ) return NULL;
			
			std::string type;
			_p.value("type", type);
			
			if (!type.compare("Equal")) {
				return new A< Op::Equal<T> >(_p);
			}else if (!type.compare("Great")) {
				return new A< Op::Great<T> >(_p);											
			}else if (!type.compare("Less")) {
				return new A< Op::Less<T> >(_p);											
			}else if (!type.compare("Differ")) {
				return new A< Op::Differ<T> >(_p);											
			}
			
			return NULL;
		}
		
		private:
			Parse _p;
	};

}
	
typedef ProductFactory<std::string, MakeFunction::Maker<Action> > ActionFactory;
typedef ProductFactory<std::string, MakeFunction::Maker<Condition> > ConditionFactory;

#endif
