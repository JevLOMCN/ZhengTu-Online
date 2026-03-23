#include "boxCircle.h"
#include "zTime.h"


int BinarySearch(std::vector<boxObject> &array,unsigned int key)  
{  
	int low = 0;  
	int high = array.size() - 1;  
	int mid = 0;  
	while ( low <= high )  
	{  
		mid = (low + high )/2;  
		if(array[mid].level == key)
			break;
		else if ( mid < high && array[mid].level < key && array[mid+1].level <= key )
			low = mid + 1;  
		else if ( mid > 0 && array[mid].level > key && array[mid-1].level >= key )     
			high = mid - 1;  
		else
		{
			if(mid<high && mid > 0)
				mid += 1;
			break;
		} 
	} 
	int mix = mid;
	int min = mid;
	while(mix < high)
	{
		if(array[mid].level == array[mix+1].level)
			++mix;
		else
			break;
	}
	while(min > 0)
	{
		if(array[mid].level == array[min-1].level)
			--min;
		else
			break;
	}
	if (mix != mid || min != mid)
		mid = zMisc::randBetween(min,mix);

	return mid;  
}

boxCircle *boxCircle::instance = NULL;

box *boxCircle::generateOneBox(unsigned int keyID,int sex,int level)
{

	struct giftBox keyBox;
	std::vector<struct giftBox>::iterator it = globalBox::getInstance()._giftBox.begin();
	std::vector<struct giftBox>::iterator end = globalBox::getInstance()._giftBox.end();
	bool isKeyBox = false;
	for( ; it != end; ++it)
	{
		if((*it)._boxitem.itemid == keyID)
		{
			keyBox = (*it);
			isKeyBox = true;
		}
	}
	//Zebra::logger->debug("boxID:%d",keyBox._boxitem.itemid);
	if (!isKeyBox)
		return NULL;
	
	box *_box = new box;
	WORD okItem = 0;
	do
	{
		for(int i = 0; i < keyBox._qualitys.size(); i++)
		{
			//Zebra::logger->debug("id:%d",keyBox._qualitys[i].quality);
			if(zMisc::selectByLakh(eyBox._qualitys[i].qualityodds))
			{	
				for (int j = 0; j < keyBox._qualitys[i].maxnum; j++)
				{
					int munkind = 1;
					if(keyBox._qualitys[i].numkind > 1)
						munkind = zMisc::randBetween(1,keyBox._qualitys[i].numkind);
					Zebra::logger->debug("munkind:%d",munkind);
					std::map<unsigned int,struct boxItem> *_boxItems = &(globalBox::getInstance()._boxItems);
					struct boxItem _boxItem =  (*_boxItems)[keyBox._qualitys[i].quality];

					if(sex == 1)
					{
						if(_boxItem.levelObjectsmale.count(munkind))
						{
							std::vector<boxObject> kindItem = _boxItem.levelObjectsmale[munkind];
							boxObject randItem = kindItem[zMisc::randBetween(0,kindItem.size()-1)];
							if(randItem.level == 0)
							{
								_box->_boxObjects[okItem] = randItem;
								okItem ++;
								//Zebra::logger->debug("objid:%d,level:%d,kind.size:%d",randItem.obj_id,randItem.level,kindItem.size());
							}else
							{
								int index = BinarySearch(kindItem,level);
								_box->_boxObjects[okItem] = kindItem[index];
								okItem ++;
							}
							//Zebra::logger->debug("okItem:%d",okItem);
							if(okItem == 17)
									return _box;
						}
					}
				}
			}	
		}
	} while (okItem > 0 && okItem < 17);
	return _box;
}
