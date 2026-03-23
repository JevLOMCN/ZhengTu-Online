#include "zDatabaseManager.h"
#include "GlobalBox.h"
#include "zXMLParser.h"

//#include "globalBox.h"
globalBox *globalBox::instance = NULL;

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

void globalBox::addBoxItem(boxItem _boxItem)
{
	//Zebra::logger->error("_boxItem.qualityID = %u\n",_boxItem.qualityID);
	//Zebra::logger->debug("_boxItem.levelObjectsmale:%d",_boxItem.levelObjectsmale.size());
	_boxItems[_boxItem.qualityID] = _boxItem;
	//_boxItems.push_back(_boxItem);
}

bool globalBox::init()
{
	zXMLParser configParser;
	Zebra::logger->trace("-------------------梦加载宝盒数据成功!-------------------");
	if(!configParser.initFile("./GiftBoxConfig.xml"))
		return false;


	xmlNodePtr rootNode = configParser.getRootNode("giftboxs");
	if(NULL == rootNode)
		return false;

	xmlNodePtr currentNode = configParser.getChildNode(rootNode,NULL);
	
	while(NULL != currentNode)
	{
		if(!xmlStrcmp(currentNode->name,(const xmlChar *)"item"))
		{

			boxItem _boxItem;
			configParser.getNodePropNum(currentNode,"quality",&_boxItem.qualityID,sizeof(_boxItem.qualityID));

			xmlNodePtr subNode = configParser.getChildNode(currentNode,"object");

			if(NULL == subNode)
				continue;
			while(subNode != NULL)
			{


				if(!xmlStrcmp(subNode->name,(const xmlChar *)"object"))
				{
					boxObject _boxObject;
				
					configParser.getNodePropNum(subNode,"obj_id",&_boxObject.obj_id,sizeof(_boxObject.obj_id));
					configParser.getNodePropNum(subNode,"num",&_boxObject.num,sizeof(_boxObject.num));
					configParser.getNodePropNum(subNode,"level",&_boxObject.level,sizeof(_boxObject.level));
					configParser.getNodePropNum(subNode,"kind",&_boxObject.kind,sizeof(_boxObject.kind));
					configParser.getNodePropNum(subNode,"itemkind",&_boxObject.itemkind,sizeof(_boxObject.itemkind));
					configParser.getNodePropNum(subNode,"itemlevel",&_boxObject.itemlevel,sizeof(_boxObject.itemlevel));
					configParser.getNodePropNum(subNode,"material",&_boxObject.material,sizeof(_boxObject.material));
					configParser.getNodePropNum(subNode,"sex",&_boxObject.sex,sizeof(_boxObject.sex));

					if( _boxObject.sex == 1)
						{
							if(objectbm.get(_boxObject.obj_id))
								_boxItem.levelObjectsmale[_boxObject.kind].push_back(_boxObject);
						}
					else if(_boxObject.sex == 2)
						{
							if(objectbm.get(_boxObject.obj_id))
								_boxItem.levelObjectsfamale[_boxObject.kind].push_back(_boxObject);
						}
					else
						{
							if(objectbm.get(_boxObject.obj_id))
							{
								_boxItem.levelObjectsmale[_boxObject.kind].push_back(_boxObject);
								_boxItem.levelObjectsfamale[_boxObject.kind].push_back(_boxObject);
							}
						}
				}

				subNode = configParser.getNextNode(subNode,NULL);
			}

			std::map<unsigned int,std::vector<boxObject> >::iterator iter=_boxItem.levelObjectsmale.begin();
			while(iter != _boxItem.levelObjectsmale.end()){
				stable_sort((*iter).second.begin(),(*iter).second.end());
				++iter;
			}

			iter=_boxItem.levelObjectsfamale.begin();
			while(iter != _boxItem.levelObjectsfamale.end()){
				stable_sort((*iter).second.begin(),(*iter).second.end());
				++iter;
			}

			//std::vector<boxObject>::iterator it = _boxItem.levelObjectsfamale.begin();
			//_boxItem.levelObjectsfamale.
			//stable_sort(_boxItem.levelObjectsfamale.begin(),_boxItem.levelObjectsfamale.end());

			/* Zebra::logger->debug("quality %d",_boxItem.qualityID);
			std::vector<boxObject>::iterator it = _boxItem.levelObjectsfamale.begin();
			std::vector<boxObject>::iterator end = _boxItem.levelObjectsfamale.end();
			for( ; it != end; ++it)
			{
				Zebra::logger->debug("needlevel %d",it->level);
			} */

			//stable_sort(_boxItem.levelObjectsmale.begin(),_boxItem.levelObjectsmale.end()); 
			addBoxItem(_boxItem);
		}
		else if(!xmlStrcmp(currentNode->name,(const xmlChar *)"giftbox"))
		{


			giftBox targetBox;
			
			int boxID;
			configParser.getNodePropNum(currentNode,"id",&boxID,sizeof(boxID));
			targetBox._boxType = boxID;
			xmlNodePtr subNode = configParser.getChildNode(currentNode,NULL);
			while(subNode != NULL)
			{


				if(!xmlStrcmp(subNode->name,(const xmlChar *)"qualitys")) //物品选择 几率
				{
					//boxObject _boxObject;
					//_boxObject.obj_id = 
				
					qualitys _qualitys;

					configParser.getNodePropNum(subNode,"quality",&_qualitys.quality,sizeof(_qualitys.quality));
					configParser.getNodePropNum(subNode,"qualityodds",&_qualitys.qualityodds,sizeof(_qualitys.qualityodds));
					configParser.getNodePropNum(subNode,"maxnum",&_qualitys.maxnum,sizeof(_qualitys.maxnum));
					configParser.getNodePropNum(subNode,"numkind",&_qualitys.numkind,sizeof(_qualitys.numkind));
					//Zebra::logger->error("quality = %u\n",_qualitys.quality);
					
					targetBox._qualitys.push_back(_qualitys);
				}else if(!xmlStrcmp(subNode->name,(const xmlChar *)"boxitem")) //钥匙
				{
					configParser.getNodePropNum(subNode,"itemid",&targetBox._boxitem.itemid,sizeof(targetBox._boxitem.itemid));
					configParser.getNodePropNum(subNode,"size",&targetBox._boxitem.size,sizeof(targetBox._boxitem.size));
					configParser.getNodePropNum(subNode,"maxnumber",&targetBox._boxitem.maxnumber,sizeof(targetBox._boxitem.maxnumber));
					configParser.getNodePropNum(subNode,"price",&targetBox._boxitem.price,sizeof(targetBox._boxitem.price));
					//Zebra::logger->error("itemid = %u\n",targetBox->_boxitem.itemid);
				}

				subNode = configParser.getNextNode(subNode,NULL);

			}

			_giftBox.push_back(targetBox);
		}

		currentNode = configParser.getNextNode(currentNode,NULL);

	}

	/* std::map<unsigned int,struct boxItem>::iterator it1 =  _boxItems.begin();
	std::map<unsigned int,struct boxItem>::iterator end1 =  _boxItems.end();
	for( ; it1 != end1; ++it1)
	{
		std::map<unsigned int,std::vector<boxObject> >::iterator it2 = it1->second.levelObjectsmale.begin();
		std::map<unsigned int,std::vector<boxObject> >::iterator end2 = it1->second.levelObjectsmale.end();
		for( ; it2 != end2; ++it2)
		{
			std::vector<boxObject>::iterator it3 = it2->second.begin();
			std::vector<boxObject>::iterator end3 = it2->second.end();
			for( ; it3 != end3; ++it3)
			{
				Zebra::logger->debug("levle:%d",it3->level);
			}
		}
	} */


	/* std::vector<boxObject>::iterator it = _boxItems[7].levelObjectsmale[1].begin();
	std::vector<boxObject>::iterator end = _boxItems[7].levelObjectsmale[1].end();
	for( ; it != end; ++it)
	{
		Zebra::logger->debug("needlevel %d",it->level);
	}
 */

	/*std::map<unsigned int,struct boxItem>::iterator boxItemIt = _boxItems.begin();
	std::map<unsigned int,struct boxItem>::iterator boxItemEnd = _boxItems.end();
	for( ; boxItemIt != boxItemEnd; ++boxItemIt)
	{
		Zebra::logger->error("------------------------- qualityID = %u -----------------------\n",(boxItemIt->second).qualityID);

		std::map<unsigned int,std::vector<boxObject>*>::iterator objIt = (boxItemIt->second)._boxObjects.begin();
		std::map<unsigned int,std::vector<boxObject>*>::iterator objEnd = (boxItemIt->second)._boxObjects.end();
		for( ; objIt != objEnd; ++objIt)
		{
			Zebra::logger->error("kind = %u\n",objIt->first);

			std::vector<boxObject>::iterator itv = objIt->second->begin();
			std::vector<boxObject>::iterator itend = objIt->second->end();

			for( ; itv != itend; ++itv)
				Zebra::logger->error("obj_id = %u\n",(*itv).obj_id);
		}
	}*/
	

	return true;
}

box *globalBox::generateOneBox(unsigned int keyID,int sex,int level)
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
			if(zMisc::selectByLakh(keyBox._qualitys[i].qualityodds))
			{	
				std::vector<int> dumpkind;
				if(keyBox._qualitys[i].numkind > 1)
				{
					for (int kindi = 1; kindi <= keyBox._qualitys[i].numkind; kindi++)
						dumpkind.push_back(kindi);	

					srand((int)time(0));
					random_shuffle(dumpkind.begin(), dumpkind.end());
				}
				for (int j = 0; j < keyBox._qualitys[i].maxnum; j++)
				{
					int munkind = 1;
					if(keyBox._qualitys[i].numkind > 1)
						munkind = dumpkind[j];
					//Zebra::logger->debug("munkind:%d",munkind);
					
					std::map<unsigned int,std::vector<boxObject> >* levelObject = NULL;
					if(sex == 1)
						levelObject = &(globalBox::getInstance()._boxItems[keyBox._qualitys[i].quality].levelObjectsmale);
					else
						levelObject = &(globalBox::getInstance()._boxItems[keyBox._qualitys[i].quality].levelObjectsfamale);
					if(levelObject && levelObject->count(munkind))
					{
						std::vector<boxObject> kindItem = (*levelObject)[munkind];
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
	} while (okItem > 0 && okItem < 18);

	return _box;
}
