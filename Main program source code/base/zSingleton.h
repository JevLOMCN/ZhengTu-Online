/**
 * \file
 * \version  $Id: zSingleton.h  $
 * \author  
 * \date 
 * \brief 
 *
 * 
 */

#ifndef _SINGLETON_H
#define _SINGLETON_H

template <typename T>
class SingletonFactory
{
	public:
		static T* instance()
		{
			return new T();
		}
};

template <typename T, typename MANA = SingletonFactory<T> > 
class Singleton
{
	private:
		/**
		 * \brief 拷贝构造函数，没有实现，禁用掉了
		 *
		 */
		Singleton(const Singleton&);

		/**
		 * \brief 赋值操作符号，没有实现，禁用掉了
		 *
		 */
		const Singleton & operator= (const Singleton &);
	protected:

		static T* ms_Singleton;
		Singleton( void )
		{
		}

		~Singleton( void )
		{
		}

	public:
	
		static void delMe(void)
		{//可以在子类的destoryMe中被调用
			if (ms_Singleton)
			{
				delete ms_Singleton;
				ms_Singleton = 0;
			}
		}

		static T* instance( void )
		{
			if (!ms_Singleton)
			{
				ms_Singleton = MANA::instance();
			}
			
			return ms_Singleton;
		}

		static T& getMe(void)
		{
			return *instance();
		}

};

template <typename T, typename MANA>
T* Singleton<T,MANA>::ms_Singleton = 0;

#endif

