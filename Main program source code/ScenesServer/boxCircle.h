
/******************************

brief:宝盒生成


*******************************/



#ifndef _BOXCIRCLE_H
#define _BOXCIRCLE_H

#include "GlobalBox.h"


//生成的一个宝盒,内含17个物品的id
struct box
{
	//DWORD items[17];
	//DWORD level[17];
	boxObject _boxObjects[17];
};



//根据配置文件定义的概率,利用转盘的方法选取出装备填充到宝盒上

class boxCircle
{
		
public:
	//根据配置文件生成一个宝盒
	box *generateOneBox(unsigned int keyID,int sex,int level);


public:
	/**
     * \brief 得到唯一实例
     *
     */
    static boxCircle &getInstance()
    {
        if (NULL == instance)
            instance = new boxCircle();

        return *instance;
    }

    /**
     * \brief 卸载唯一实例
     *
     */
    static void delInstance()
    {
        SAFE_DELETE(instance);
    }

private:
	boxCircle()
	{
		//createBoxCircle();
	}
	~boxCircle(){}

	static boxCircle *instance;

};


#endif
