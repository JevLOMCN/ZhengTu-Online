#ifndef _GIFTBOX_H
#define _GIFTBOX_H

#include <vector>
#include <map>
//#include <list>
#include "zDatabase.h"


class boxCircle;

//箱子处理

/* enum boxType
{
	golden = 0, //金箱
	silver,//银箱
}; */


struct qualitys
{
	unsigned int quality;
	unsigned int qualityodds;
	unsigned int maxnum;
	unsigned int numkind;
};

struct boxitem
{
	unsigned int itemid;
	unsigned int size;
	unsigned int maxnumber;
	unsigned int price;
};

struct giftBox
{
	
	unsigned int _boxType;
	boxitem _boxitem;
	std::vector<struct qualitys> _qualitys;
	
};

//object 物品详细
struct boxObject
{
	operator int() const 
	{
		return (int)level;
	}
	unsigned int obj_id;
	unsigned int num;
	unsigned int level;
	unsigned int kind;
	unsigned int itemkind;
	unsigned int itemlevel;
	unsigned int material;
	unsigned char sex;
};

//item 物品大类 <item quality="物品档次划分">
struct boxItem
{
	unsigned int qualityID;
	//std::map<unsigned int,std::vector<boxObject>*> _boxObjects;
	
	std::map<unsigned int,std::vector<boxObject> > levelObjectsmale;//与等级相关的物品,男性	
	std::map<unsigned int,std::vector<boxObject> > levelObjectsfamale;//与等级相关的物品,女性	
};

//生成的一个宝盒,内含17个物品的id
struct box
{
	//DWORD items[17];
	//DWORD level[17];
	boxObject _boxObjects[17];
};


class globalBox
{
public:	
	bool init();

    /**
     * \brief 得到唯一实例
     *
     */
    static globalBox &getInstance()
    {
        if (NULL == instance)
            instance = new globalBox();

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

	const giftBox& getGlodenBox()
	{
		return glodenBox;
	}

	const giftBox& getSilverBox()
	{
		return silverBox;
	}

	

private:
	void addBoxItem(boxItem _boxItem);

    static globalBox *instance;


private:
	globalBox(){};
	~globalBox(){};

	
public:
	struct giftBox glodenBox;
	struct giftBox silverBox;
	std::vector<struct giftBox> _giftBox;
	//所有物品 物品大类  物品详细
	std::map<unsigned int,struct boxItem> _boxItems;

	box* generateOneBox(unsigned int keyID,int sex,int level);
		
};


#endif
