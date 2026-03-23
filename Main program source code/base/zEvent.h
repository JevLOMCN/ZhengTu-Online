/**
 * \file	zEvent.h
 * \version  	$Id$
 * \author  	
 * \date 	
 * \brief 	定义FSM的事件接口
 *
 * 
 */

#ifndef _EVENT_H
#define _EVENT_H

#include <string>

/**
 * \brief 有限状态机的事件接口
 *
 */
class Event 
{
	public:

		explicit Event (const std::string ename)
		{
			size = 0;
			data = NULL;
			name = ename;
		}

		/**
		 * \brief 拷贝构造函数
		 *
		 */
		Event(Event& ref)
		{
			bcopy(ref.data, data, ref.size);
			this->size = ref.size;
			name = ref.name;
		}

		virtual ~Event()
		{
			if (data) 
			{
				delete[] data;
				data = NULL;
			}
		}

	/**
	 * 返回事件的名称
	 */

	/**
	 * \brief 返回事件的名称
	 *
	 */
	virtual std::string get_name()
	{
		return this->name;
	}

	/**
	 * \brief 设置该事件相关的数据 
	 *
	 *
	 * \param ptNullCmd 数据指针
	 * \return 
	 */
	virtual void set_data(const unsigned char *buf, const unsigned int buf_size)
	{
		this->size = buf_size;
		this->data = new unsigned char [this->size];
		bcopy(buf, this->data, this->size);
	}

	/**
	 * \brief  获取该事件相关的数据
	 *
	 *
	 * \param buf 数据保存在此
	 * \param size 数据大小
	 *
	 * \return 有返回TRUE,无返回FALSE
	 */
	virtual void get_data(unsigned char * buf, int * size)
	{
		bcopy(this->data, buf, this->size);
		*size = this->size;
	}
	
	/**
	 * \brief  获取该事件相关的数据
	 *
	 * \return 返回数据
	 */
	virtual unsigned char* get_data()
	{
		return data;
	}
	
	/**
	 * \brief  获取该事件相关的数据大小
	 *
	 * \return 返回数据大小
	 */
	virtual unsigned int get_size()
	{
		return size;
	}

	/**
	 * \brief 该事件是否有效
	 *
	 * \return 有效,返回TRUE, 否则,返回FALSE
	 */
	bool is_valid()
	{
		return valid;
	}

	/**
	 * \brief 重载赋值运算符
	 *
	 *
	 * \param 
	 * \return 
	 */
	Event& operator= (Event& ref)
	{
		bcopy(ref.data, data, ref.size);
		this->size = ref.size;
		this->name = ref.name;
		return *this;
	}
	
	protected:
		unsigned char* data;
		unsigned int   size;
		std::string    name;

	private:
		bool valid;
};

#endif
