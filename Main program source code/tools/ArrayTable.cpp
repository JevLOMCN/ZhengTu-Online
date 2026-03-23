#include "ArrayTable.h"
#include "zXMLParser.h"





ArrayTable *ArrayTable::instance = NULL;



bool ArrayTable::init(unsigned int type)
{
	final();
	zXMLParser xml;
	if(!xml.initFile(Zebra::global["skilltable"]))
	{
		Zebra::logger->debug("加载职业技能失败!");
		return false;
	}

	xmlNodePtr root = xml.getRootNode("Zebra");
	if(root)
	{
		xmlNodePtr subroot = xml.getChildNode(root , "Skill");
		if(subroot)
		{
			xmlNodePtr node = xml.getChildNode(subroot , "type");
			while(node)
			{
				if(strcmp((char*)node->name , "type") == 0)
				{
					DWORD t;
					xml.getNodePropNum(node , "id" , &t , sizeof(t));
					xmlNodePtr sub = xml.getChildNode(node , "value");
					while(sub)
					{
						if(strcmp((char*)sub->name , "value") == 0)
						{
							DWORD v;
							xml.getNodePropNum(sub , "var" , &v , sizeof(v));
							array.push_back(v);
						}
						sub = xml.getNextNode(sub , NULL);
					}
				}
				else
				{
					Zebra::logger->debug("type err:%s",(char*)node->name);
				}
				node = xml.getNextNode(node , NULL);
			}
		}
		else
		{
			Zebra::logger->debug("加载技能失败");
		}
		subroot = xml.getChildNode(root , "Timer");
		if(subroot)
		{
			xmlNodePtr node = xml.getChildNode(subroot , "frequency");
			while(node)
			{
				if(strcmp((char*)node->name , "frequency") == 0)
				{
					DWORD t;
					xml.getNodePropNum(node , "fre" , &t , sizeof(t));
					xmlNodePtr sub = xml.getChildNode(node , "operation");
					str_vec v;
					while(sub)
					{
						if(strcmp((char*)sub->name , "operation") == 0)
						{
							char Buf[256];
							bzero(Buf , sizeof(Buf));
							xml.getNodePropStr(sub , "oper" , Buf , sizeof(Buf));
							v.push_back(Buf);
						}
						sub = xml.getNextNode(sub , NULL);
					}
					operation.insert(vec_map_value_type(t , v));
				}
				else
				{
					Zebra::logger->debug("frequency err:%s",(char*)node->name);
				}
				node = xml.getNextNode(node , NULL);
			}
		}
		else
		{
			Zebra::logger->debug("加载操作失败");
		}
		subroot = xml.getChildNode(root , "Global");
		if(subroot)
		{
			xmlNodePtr child=xml.getChildNode(subroot,NULL);
			while(child)
			{
				char buf[128];
				if(xml.getNodeContentStr(child,buf,128))
				{
					global[(char *)child->name]=buf;
				}
				child=xml.getNextNode(child,NULL);
			}
		}
	}
	// */
	return true;
}
void ArrayTable::clearOperation()
{
	for(vec_map::iterator iter = operation.begin() ; iter != operation.end() ; iter ++)
	{
		iter->second.clear();
	}
	//operation.clear();
}
DWORD ArrayTable::get(DWORD which)
{
	if(which < array.size())
	{
		return array[which];
	}
	return 0;
}
ArrayTable *ArrayTable::getInstance()
{
	if(instance == NULL)
	{
		instance = new ArrayTable();
	}
	return instance;
}
void ArrayTable::delInstance()
{
	if(instance)
	{
		delete instance;
		instance=NULL;
	}
}
