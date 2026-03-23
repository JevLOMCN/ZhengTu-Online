#include "public.h"
#include "table.h"
#include <algorithm>
#include "command.h"
#include "../xml_parse/XmlParseEx.h"

#ifdef _AUTOPLAY
#include "./AutoGame.h"
#endif

const int c_nGAME_VERSION = 20151025;// 这个参数作废，新参数定义在Game.cpp中
// ===============================================================================
static stCharTypeInfo c_charTypeInfo[3] =
{
	{PROFESSION_NONE,0,1,1,"无业",eSex_Male,{0}},

	{PROFESSION_1,0,1,1,"侠客",eSex_Male,{0}},
	{PROFESSION_2,1,2,2,"侠女",eSex_Female,{0}},
};

// ===============================================================================
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param type : 描述
 * \return 返回值的描述
 */
const stCharTypeInfo & GetCharTypeInfo(size_t type)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<count_of(c_charTypeInfo);++i)
	{
		if(type == c_charTypeInfo[i].type)
			return c_charTypeInfo[i];
	}
	return c_charTypeInfo[0];

	FUNCTION_END;
}
// ===============================================================================
tTableObjectBase g_tableObjectBase;
tTableNpcBase	 g_tableNpcBase;
tTableSkillBase	 g_tableSkillBase;
tTableSkillLevelBase	 g_tableSkillLevelBase;
tTableObjectUp  	 g_tableObjectUp;
tTableItemSetBase		g_tableItemSetBase;
tTableCharStateBase	g_tableCharStateBase;
tTableHairStyle		g_tableHairStyle;
tTableHairColor		g_tableHairColor;
tTableKnight        g_tableKnight;
tTableCivilian      g_tableCivilian;
tTableHeadList          g_tableHeadList;
tLevelUpTipMap		g_mapLevelup;
tTableCountryMaterialList g_CountryMatrialList;
tShenjianList		g_jianzhong;
tZtzList		g_ztz;
tShengxiaoList		g_shengxiao;
tShengqiList		g_shengqi;
tJingmaiList		g_jingmai;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param data1 : 描述
 * \param data2 : 描述
 * \return 返回值的描述
 */
template <class tRecord>
inline bool DataLessById(tRecord& data1,tRecord & data2)
{
	FUNCTION_BEGIN;

	return data1.dwID < data2.dwID;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param data1 : 描述
 * \param data2 : 描述
 * \return 返回值的描述
 */
bool SkillLevelDataLessById(SkillLevelBase_t & data1,SkillLevelBase_t& data2)
{
	FUNCTION_BEGIN;

	if(data1.dwID == data2.dwID)
	{
		return data1.dwLevel < data2.dwLevel;
	}
	return data1.dwID <= data2.dwID;

	FUNCTION_END;
}

// ===============================================================================
// 获得国王的名称
char*  GetKnightName(DWORD value)
{
	for (int i=0;i<g_tableKnight.size();i++)
	{
		if (value<g_tableKnight[i].dwValue)
		{
			if (i>0)
				return g_tableKnight[i-1].strName;
			else
				return "";
		}
	}
	return g_tableKnight[g_tableKnight.size()-1].strName;
}

// ===============================================================================
// 获得平民的名称
char* GetCivilianName(DWORD value)
{
	for (int i=0;i<g_tableCivilian.size();i++)
	{
		if (value<g_tableCivilian[i].dwValue)
		{
			if (i>0)
				return g_tableCivilian[i-1].strName;
			else
				return "";
		}
	}
	return g_tableCivilian[g_tableCivilian.size()-1].strName;
}
/**
* \brief 简短描述
* 
* 详细描述
* 
* \param data1 : 描述
* \param data2 : 描述
* \return 返回值的描述
*/
bool SkillLevelDataLessByIdTree(SkillLevelBase & data1,SkillLevelBase& data2)
{
	FUNCTION_BEGIN;

	if(data1.dwID == data2.dwID)
	{
		return data1.dwLevel < data2.dwLevel;
	}
	return data1.dwID <= data2.dwID;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pStream : 描述
 * \param vecData : 描述
 * \return 返回值的描述
 */
template <class tRecord>
bool ReadTableData(Stream* pStream,std::vector<tRecord> & vecData)
{
	FUNCTION_BEGIN;

	DWORD dwCount;
	pStream->read(&dwCount,sizeof(dwCount));
	vecData.resize(dwCount);
	pStream->read(&vecData[0],sizeof(tRecord) * dwCount);
	std::sort(vecData.begin(),vecData.end(),DataLessById<tRecord>);
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pStream : 描述
 * \param vecData : 描述
 * \return 返回值的描述
 */
bool ReadSkillLevelTableData(Stream* pStream,std::vector<SkillLevelBase_t> & vecData)
{
	FUNCTION_BEGIN;

	DWORD dwCount;
	pStream->read(&dwCount,sizeof(dwCount));
	vecData.resize(dwCount);
	pStream->read(&vecData[0],sizeof(SkillLevelBase_t) * dwCount);
	std::sort(vecData.begin(),vecData.end(),SkillLevelDataLessById);
	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param pStream : 描述
* \param vecData : 描述
* \return 返回值的描述
*/
bool ReadSkillLevelTableDataTree(Stream* pStream,std::vector<SkillLevelBase> & vecData)
{
	FUNCTION_BEGIN;

	DWORD dwCount;
	pStream->read(&dwCount,sizeof(dwCount));
	vecData.resize(dwCount);
	pStream->read(&vecData[0],sizeof(SkillLevelBase) * dwCount);
	std::sort(vecData.begin(),vecData.end(),SkillLevelDataLessByIdTree);
	return true;

	FUNCTION_END;
}

bool LoadLevelupTipMap()
{
	FUNCTION_BEGIN;

	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\leveluptip.xml");
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* Tips = doc.FirstChildElement("Tips");
		if( Tips )
		{
			TiXmlElement* Tip = Tips->FirstChildElement("Tip");
			while( Tip )
			{
				int level = 0;
				if( Tip->QueryIntAttribute("level",&level) != TIXML_SUCCESS )
				{
					level = 0;
				}
				const char* item = Tip->Attribute("item");
				if( NULL != item && strlen(item) > 0 )
				{
					g_mapLevelup.insert(tLevelUpTipMap::value_type(level,item));
				}				
				Tip = Tip->NextSiblingElement("Tip");
			}
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	
	FUNCTION_END;
}


bool LoadShenjianConfig() //加载剑冢配置 by醉梦
{
	FUNCTION_BEGIN;
	g_jianzhong.clear();
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\ShenJianConfig.xml");
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* config = doc.FirstChildElement("config");
		if( config )
		{
			TiXmlElement* meridianMap = config->FirstChildElement("meridianMap");
			if(meridianMap)
			{
				TiXmlElement* meridian = meridianMap->FirstChildElement("meridian");
				while( meridian )
				{
					int index = 0;
					meridian->QueryIntAttribute("index",&index);	
					if(index==1)
					{
						TiXmlElement* pointMap = meridian->FirstChildElement("pointMap");
						if(pointMap)
						{
							TiXmlElement* point = pointMap->FirstChildElement("point");
							while( point )
							{
								int level = 0;
								point->QueryIntAttribute("level",&level);	
								int value = 0;
								point->QueryIntAttribute("value",&value);	
								int costID = 0;
								point->QueryIntAttribute("costID",&costID);	
								int costNum = 0;
								point->QueryIntAttribute("costNum",&costNum);	
								int salary = 0;
								point->QueryIntAttribute("salary",&salary);	
								int pDam = 0;
								point->QueryIntAttribute("pDam",&pDam);	
								int mDam = 0;
								point->QueryIntAttribute("mDam",&mDam);	
								int pDef = 0;
								point->QueryIntAttribute("pDef",&pDef);	
								int mDef = 0;
								point->QueryIntAttribute("mDef",&mDef);	
								int hp = 0;
								point->QueryIntAttribute("hp",&hp);	
								shenjianpeizhi sjpz;
								sjpz.level = level;
								sjpz.value = value;
								sjpz.costID = costID;
								sjpz.costNum = costNum;
								sjpz.salary = salary;
								sjpz.pDam = pDam;
								sjpz.mDam = mDam;
								sjpz.pDef = pDef;
								sjpz.mDef = mDef;
								sjpz.hp = hp;
								g_jianzhong.push_back(sjpz);
								point = point->NextSiblingElement("point");
							}
						}

					}	
					meridian = meridian->NextSiblingElement("meridian");
				}
			}
			
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	
	FUNCTION_END;
}

bool LoadZtzConfig() //加载征途转配置 by醉梦 这里我写好了 我给你解释代码
{
	FUNCTION_BEGIN;
	//首先 我们加载之前先给 列表执行.clear()方法 顾名思义 clear英文意思 清理  也就是 我们先把 里面的内容 清空 以免出现某种问题造成重复加载的情况出现 懂了吗
	g_ztz.clear();
	
	//这里 固定用法 直接复制 
	TiXmlDocument doc;
	//这里 我们 需要用到 OpenPackFileForRead 也是固定用法 不需要记住 需要的时候去上面复制即可  这个是取xml的位置也就是读取  会把xml以流的形式读取到pStream 内
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\ZhengTuZhuanConfig.xml");
	//这里 ！pStream 还记得我教你的 与或非表达式吗 这个就是 非 也就是 当 pStream 是空的时候 执行 或者 不存在的时候 执行 
	//那么什么事不存在 或空 那就是没有这个文件被ZhengTuZhuanConfig  如果是空 则直接 return 回去 flase 否  return 你知道是什么意思吗
	//return 是返回的意思 你目前不涉及到return传值 所以 你可以暂时先理解为 代码运行的时候 一旦遇到return 就代表着你这一段代码结束了
	//代码运行是逐行运行 走到了 return这 下面代码就不会在执行了 就相当于结束了 这一块 懂了吗
	if(!pStream) return false;	
	//这里也是判断 判断的是LoadXMLFromStream 以xml格式去加载文件 是否成功 如过是true 进入
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		//我们取值的时候 要按照xml的格式来取值 他这里是一层套一层的 首先先取最外层的 先取config
		TiXmlElement* config = doc.FirstChildElement("config");
		if( config )
		{
			TiXmlElement* meridianMap = config->FirstChildElement("meridianMap");//再去config里取meridianMap
			if(meridianMap)
			{
				TiXmlElement* meridian = meridianMap->FirstChildElement("meridian");//同理 meridianMap里取meridian 懂了吗
				while( meridian )//这里是循环 因为这个配置文件是做的10个图鉴分开的属性 不过我这段代码 还没用到 我目前用的是 10个图鉴公用一个属性 
				{
					int index = 0;
					meridian->QueryIntAttribute("index",&index);	
					if(index==1)//所以这里我写了index=1 后面的 2-10 我们先不管他 现在用不到他
					{
						TiXmlElement* pointMap = meridian->FirstChildElement("pointMap");
						
						if(pointMap)
						{
							TiXmlElement* point = pointMap->FirstChildElement("point"); //直接看这里 继续meridian 里取point 
							//point 就是我们属性的标签了 对不 所以 我们直接开始循环
							//这里就接触到了 我们编程里做常用的东西 循环 这里我们用的是while循环 不算长用 一会会有一个for循环那个才是重中之重  这个我告诉你原理是什么

							//int a=0;
							//我们定义一个a变量 他初始值为0
							// while(a<100) //while循环 简单来说 就是 while后面的括号里面的内容 不成立的时候 才会跳出循环 懂了吗
							//说错了.....是括号里的内容 不成立的时候 跳出循环 a<100 当a变成100的时候 跳出循环
							// {
							// 	a++;
							// }








							//又回到了这里 继续判断 point还存不存在？ 第二次 肯定存在对吧 然后 继续 一直循环到 下一个point 不存在之后 跳出循环
							while( point ) //好 接下来 条件是什么 条件是 point 不是空的时候 就会一直去执行 对吧
							{
								int level = 0;
								//取值
								//这个地方 新建一个 变量 叫levelQueryIntAttribute 方法后面有两个参数 第一个参数代表 要取xml里的那一个字段 第二个参数代表 取出来存到那里 懂了吗
								//后面我就不多说了 复制粘贴
								point->QueryIntAttribute("level",&level);	
								int value = 0;
								point->QueryIntAttribute("value",&value);	
								int costID = 0;
								point->QueryIntAttribute("costID",&costID);	
								int costNum = 0;
								point->QueryIntAttribute("costNum",&costNum);	
								int salary = 0;
								point->QueryIntAttribute("salary",&salary);	
								int pDam = 0;
								point->QueryIntAttribute("pDam",&pDam);	
								int mDam = 0;
								point->QueryIntAttribute("mDam",&mDam);	
								int pDef = 0;
								point->QueryIntAttribute("pDef",&pDef);	
								int mDef = 0;
								point->QueryIntAttribute("mDef",&mDef);	
								int hp = 0;
								point->QueryIntAttribute("hp",&hp);	
								ztzpeizhi pz; //好 这里 最重要的 我们创建一个 类型 就是我们刚刚定义的那个取名为 pz 
								pz.level = level;
								pz.value = value;
								pz.costID = costID;
								pz.costNum = costNum;
								pz.salary = salary;
								pz.pDam = pDam;
								pz.mDam = mDam;
								pz.pDef = pDef;
								pz.mDef = mDef;
								pz.hp = hp;
								//一直到这里 都是在给pz这个类型里面的值赋值 这里您看明白了吧
								//好 当我们定义好了一个 ztzpeizhi后 我们直接给他插入到刚刚我说的那个专门存放ztzpeizhi类型的集合里面 用push_back方法 他会 自动按顺序插进去 懂了吗
								g_ztz.push_back(pz);
								//好 这是第一次循环  我们push——back后g_ztz这个集合 里面的第【0】个 就有内容了 就是这个pz 接下来
								//我们给point 读取NextSiblingElement("point"); 也就是读取下一行 懂了吧 这个很好理解
								point = point->NextSiblingElement("point");
								//好 走到这 我们循环就会回到最开始的地方
							}
							//跳到这里结束 这样 我们的100跳 配置 就已经 循环存入到g_ztz里面了
						}

					}	
					meridian = meridian->NextSiblingElement("meridian");
				}
			}
			
		}
	}
	ClosePackFileForRead(pStream);
	//这里 是关闭我们的流读取
	//不需要太多理解

	//然后 结束return
	//这一套流程理解了吗

	//现在我们只需要去调用这个方法即可
	return true;
	
	FUNCTION_END;
}

bool LoadShengxiaoConfig() //加载十二生肖配置 by醉梦
{
	FUNCTION_BEGIN;
	g_shengxiao.clear();
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\12BeastsConfig.xml");
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* config = doc.FirstChildElement("config");
		if( config )
		{
			TiXmlElement* meridianMap = config->FirstChildElement("meridianMap");
			if(meridianMap)
			{
				TiXmlElement* meridian = meridianMap->FirstChildElement("meridian");
				while( meridian )
				{
					int index = 0;
					meridian->QueryIntAttribute("index",&index);	
					if(index==1)
					{
						TiXmlElement* pointMap = meridian->FirstChildElement("pointMap");
						if(pointMap)
						{
							TiXmlElement* point = pointMap->FirstChildElement("point");
							while( point )
							{
								int level = 0;
								point->QueryIntAttribute("level",&level);	
								int value = 0;
								point->QueryIntAttribute("value",&value);	
								int costID = 0;
								point->QueryIntAttribute("costID",&costID);	
								int costNum = 0;
								point->QueryIntAttribute("costNum",&costNum);	
								int salary = 0;
								point->QueryIntAttribute("salary",&salary);	
								int pDam = 0;
								point->QueryIntAttribute("pDam",&pDam);	
								int mDam = 0;
								point->QueryIntAttribute("mDam",&mDam);	
								int pDef = 0;
								point->QueryIntAttribute("pDef",&pDef);	
								int mDef = 0;
								point->QueryIntAttribute("mDef",&mDef);	
								int hp = 0;
								point->QueryIntAttribute("hp",&hp);	
								shengxiaopeizhi sxpz;
								sxpz.level = level;
								sxpz.value = value;
								sxpz.costID = costID;
								sxpz.costNum = costNum;
								sxpz.salary = salary;
								sxpz.pDam = pDam;
								sxpz.mDam = mDam;
								sxpz.pDef = pDef;
								sxpz.mDef = mDef;
								sxpz.hp = hp;
								g_shengxiao.push_back(sxpz);
								point = point->NextSiblingElement("point");
							}
						}

					}	
					meridian = meridian->NextSiblingElement("meridian");
				}
			}
			
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	
	FUNCTION_END;
}

bool LoadShengqiConfig() //加载圣器配置 by醉梦
{
	FUNCTION_BEGIN;
	g_shengqi.clear();
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\ShengQiConfig.xml");
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* config = doc.FirstChildElement("config");
		if( config )
		{
			TiXmlElement* meridianMap = config->FirstChildElement("meridianMap");
			if(meridianMap)
			{
				TiXmlElement* meridian = meridianMap->FirstChildElement("meridian");
				while( meridian )
				{
					int index = 0;
					meridian->QueryIntAttribute("index",&index);	
					if(index==1)
					{
						TiXmlElement* pointMap = meridian->FirstChildElement("pointMap");
						if(pointMap)
						{
							TiXmlElement* point = pointMap->FirstChildElement("point");
							while( point )
							{
								int level = 0;
								point->QueryIntAttribute("level",&level);	
								int value = 0;
								point->QueryIntAttribute("value",&value);	
								int costID = 0;
								point->QueryIntAttribute("costID",&costID);	
								int costNum = 0;
								point->QueryIntAttribute("costNum",&costNum);	
								int salary = 0;
								point->QueryIntAttribute("salary",&salary);	
								int pDam = 0;
								point->QueryIntAttribute("pDam",&pDam);	
								int mDam = 0;
								point->QueryIntAttribute("mDam",&mDam);	
								int pDef = 0;
								point->QueryIntAttribute("pDef",&pDef);	
								int mDef = 0;
								point->QueryIntAttribute("mDef",&mDef);	
								int hp = 0;
								point->QueryIntAttribute("hp",&hp);	
								shengqipeizhi sqpz;
								sqpz.level = level;
								sqpz.value = value;
								sqpz.costID = costID;
								sqpz.costNum = costNum;
								sqpz.salary = salary;
								sqpz.pDam = pDam;
								sqpz.mDam = mDam;
								sqpz.pDef = pDef;
								sqpz.mDef = mDef;
								sqpz.hp = hp;
								g_shengqi.push_back(sqpz);
								point = point->NextSiblingElement("point");
							}
						}

					}	
					meridian = meridian->NextSiblingElement("meridian");
				}
			}
			
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	
	FUNCTION_END;
}

bool LoadJingmaiConfig() //加载经脉配置 by醉梦
{
	FUNCTION_BEGIN;
	g_jingmai.clear();
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\JingMaiConfig.xml");
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* config = doc.FirstChildElement("config");
		if( config )
		{
			TiXmlElement* meridianMap = config->FirstChildElement("meridianMap");
			if(meridianMap)
			{
				TiXmlElement* meridian = meridianMap->FirstChildElement("meridian");
				while( meridian )
				{
					int index = 0;
					meridian->QueryIntAttribute("index",&index);	
					if(index==1)
					{
						TiXmlElement* pointMap = meridian->FirstChildElement("pointMap");
						if(pointMap)
						{
							TiXmlElement* point = pointMap->FirstChildElement("point");
							while( point )
							{
								int level = 0;
								point->QueryIntAttribute("level",&level);	
								int value = 0;
								point->QueryIntAttribute("value",&value);	
								int costID = 0;
								point->QueryIntAttribute("costID",&costID);	
								int costNum = 0;
								point->QueryIntAttribute("costNum",&costNum);	
								int salary = 0;
								point->QueryIntAttribute("salary",&salary);	
								int pDam = 0;
								point->QueryIntAttribute("pDam",&pDam);	
								int mDam = 0;
								point->QueryIntAttribute("mDam",&mDam);	
								int pDef = 0;
								point->QueryIntAttribute("pDef",&pDef);	
								int mDef = 0;
								point->QueryIntAttribute("mDef",&mDef);	
								int hp = 0;
								point->QueryIntAttribute("hp",&hp);	
								jingmaipeizhi jmpz;
								jmpz.level = level;
								jmpz.value = value;
								jmpz.costID = costID;
								jmpz.costNum = costNum;
								jmpz.salary = salary;
								jmpz.pDam = pDam;
								jmpz.mDam = mDam;
								jmpz.pDef = pDef;
								jmpz.mDef = mDef;
								jmpz.hp = hp;
								g_jingmai.push_back(jmpz);
								point = point->NextSiblingElement("point");
							}
						}

					}	
					meridian = meridian->NextSiblingElement("meridian");
				}
			}
			
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	
	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool LoadTables()
{
	FUNCTION_BEGIN;

	LoadLevelupTipMap();
	LoadShenjianConfig();//加载神剑配置 by醉梦
	LoadZtzConfig();//加载征途转配置 by醉梦	
	LoadShengxiaoConfig();//加载十二生肖配置 by醉梦
	LoadShengqiConfig();//加载圣器配置 by醉梦
	LoadJingmaiConfig();//加载经脉配置 by醉梦

	Stream* pStream;

	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\ObjectBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<ObjectBase_t>(pStream,g_tableObjectBase)) 
	{
		ClosePackFileForRead(pStream);
		return false;
	}



	ClosePackFileForRead(pStream);
//#ifdef _DEBUG	
//	for(tTableObjectBase::iterator it = g_tableObjectBase.begin(); it != g_tableObjectBase.end(); ++it)
//	{ 
//		TRACE("object:[%u][%s][%x]\n",(*it).dwID,(*it).strName,(*it).dwType);
//		//TRACE("object:[%u][%s][%x][%u]\n",(*it).dwID,(*it).strName,(*it).dwType,(*it).dwIntervalTime);
//	}
//#endif

	//*
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\NpcBase.tbl");
	if(!pStream) return false;
	if(!ReadTableData<NpcBase_t>(pStream,g_tableNpcBase))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	////for(tTableNpcBase::iterator it = g_tableNpcBase.begin(); it != g_tableNpcBase.end(); ++it)
	//{ 
	//	/*TRACE("npc:[%d][%s][%d][%d][%d][%d][%d][%d]\n",
	//		(*it).dwID,
	//		(*it).strName,
	//		(*it).dwType,
	//		(*it).dwLevel,
	//		(*it).dwColor,
	//		(*it).dwFive,
	//		(*it).dwPic,
	//		(*it).dwImage);*/
	//}

#ifdef _AUTOPLAY
	if(gAutoGameConfig.fnLoadNpcTables){
		pStream->setPosition(0);
		std::vector<BYTE> data;
		data.resize(pStream->getSize());
		pStream->read(&data[0],data.size());
		gAutoGameConfig.fnLoadNpcTables(&data[0]);
	}
#endif

	ClosePackFileForRead(pStream);


	//*
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\ObjectUp.tbl");
	if(!pStream) return false;
	if(!ReadTableData<ObjectUp_t>(pStream,g_tableObjectUp))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);


	//*/
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\SkillBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<SkillBase_t>(pStream,g_tableSkillBase))
	{
		ClosePackFileForRead(pStream);
		return false;
	}

#ifdef _AUTOPLAY
	if(gAutoGameConfig.fnLoadSkillTables){
		pStream->setPosition(0);
		std::vector<BYTE> data;
		data.resize(pStream->getSize());
		pStream->read(&data[0],data.size());
		gAutoGameConfig.fnLoadSkillTables(&data[0]);
	}
#endif

	ClosePackFileForRead(pStream);

	
	for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		//if ( (*it).dwID == 377)
		//	TRACE( "skill:%u:%s:%u, target=%u\n",(*it).dwID,(*it).pstrName,(*it).dwSerialType,(*it).dwPic );
	}
	
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\SkillLevelBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadSkillLevelTableData(pStream,g_tableSkillLevelBase))
	{
		ClosePackFileForRead(pStream);
		return false;
	}

#ifdef _AUTOPLAY
	if(gAutoGameConfig.fnLoadSkillLevelTables){
		pStream->setPosition(0);
		std::vector<BYTE> data;
		data.resize(pStream->getSize());
		pStream->read(&data[0],data.size());
		gAutoGameConfig.fnLoadSkillLevelTables(&data[0]);
	}
#endif

	ClosePackFileForRead(pStream);

	//Read item set table
	pStream = OpenPackFileForRead( "data\\datas.pak", "datas\\tables\\SetObjectBase.tbl" );
	if( !pStream) return false;

	std::vector<ItemSetTable_t> tableItemSetSource;
	//
	if ( !ReadTableData<ItemSetTable_t>( pStream, tableItemSetSource ) )
	{
		ClosePackFileForRead( pStream );
		return false;
	}

	//Get what we want really
	for( std::vector<ItemSetTable_t>::iterator itor=tableItemSetSource.begin();itor!=tableItemSetSource.end();itor++ )
	{
		ItemSetBase setBase;
		size_t k;
		size_t num;

		setBase.dwID = itor->dwID;
		sprintf( setBase.pstrName, itor->pstrName );
		setBase.charType = itor->charType;
		//Get item id list
		std::vector<DWORD>	aParams;
		_parse_str_num( aParams, itor->pstrListID );
		num = 7;
		for( k=0;k<getMin(aParams.size(),num);k++ )
			setBase.itemList[k] = aParams[k];
		//Get effect one :: set property id list
		aParams.resize(0);
		_parse_str_num( aParams, itor->pstrEffectOneID );
		num = 3;
		for( k=0;k<getMin(aParams.size(),num);k++ )
			setBase.effectOneID[k] = aParams[k];
		//Get effect two :: set property id list
		aParams.resize(0);
		_parse_str_num( aParams, itor->pstrEffectTwoID );
		num = 5;
		for( k=0;k<getMin(aParams.size(),num);k++ )
			setBase.effectTwoID[k] = aParams[k];
		//Get all :: set property id list
		aParams.resize(0);
		_parse_str_num( aParams, itor->pstrAllID );
		num = 7;
		for( k=0;k<getMin(aParams.size(),num);k++ )
			setBase.idList[k] = aParams[k];

		//Push to global var
		g_tableItemSetBase.push_back( setBase );
	}
	//
	ClosePackFileForRead( pStream );

	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\CharacterStateBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<CharacterStateBase_t>(pStream,g_tableCharStateBase))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\HairStyle.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<HairStyle_t>(pStream,g_tableHairStyle))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);

	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\HairColour.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<HairColor_t>(pStream,g_tableHairColor))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);


	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\Knight.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<Knight_t>(pStream,g_tableKnight))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);

#ifdef _DEBUG
	for (int i=0;i<g_tableKnight.size();i++)
	{
		TRACE("%u %s %u\n",g_tableKnight[i].dwID,g_tableKnight[i].strName,g_tableKnight[i].dwValue);
	}
#endif

	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\Civilian.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<Civilian_t>(pStream,g_tableCivilian))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);


	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\HeadListBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<HeadList_t>(pStream,g_tableHeadList))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);

	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\CountryMaterial.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<CountryMaterial_t>(pStream,g_CountryMatrialList))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);


	/*for(tTableCharStateBase::iterator it = g_tableCharStateBase.begin(); it != g_tableCharStateBase.end(); ++it)
		TRACE( "CharacterState::ID=%u	PIC=%u	SID=%u\n", (*it).dwID, (*it).dwPicID, (*it).dwStateID ); */

	CIni ini;
	ini.Open("data\\datas.pak","datas\\hair.ini");
	for(size_t i=0; i < count_of(c_charTypeInfo); ++i)
	{
		char szType[64];
		sprintf(szType,"%d",c_charTypeInfo[i].type);
		for(size_t j = 0; j < 16; ++j)
		{
			char szHair[64];
			sprintf(szHair,"%d",j);
			c_charTypeInfo[i].hair[j] = ini.GetInt(szType,szHair,0);
		}
		c_charTypeInfo[i].equipHairPic = ini.GetInt(szType,"paper",0);
	}
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool ReLoadSkillTable()
{
	FUNCTION_BEGIN;

	Stream* pStream;
	pStream = OpenPackFileForRead("data\\datas.pak","datas\\tables\\SkillBase.tbl");		//"data\\datas.pak"
	if(!pStream) return false;
	if(!ReadTableData<SkillBase_t>(pStream,g_tableSkillBase))
	{
		ClosePackFileForRead(pStream);
		return false;
	}
	ClosePackFileForRead(pStream);

	/*for(tTableSkillBase::iterator it = g_tableSkillBase.begin(); it != g_tableSkillBase.end(); ++it)
	{ 
		TRACE( "skill:%u:%s, itemID=%d\n",(*it).dwID,(*it).pstrName,(*it).dwUpItemID );
	}*/
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void UnloadTables()
{
	FUNCTION_BEGIN;

	g_tableObjectBase.resize(0);
	g_tableNpcBase.resize(0);

	FUNCTION_END;
}

bool IsSelectFirstSkill(SkillBase* pSelectSkill)
{
	if ( NULL == pSelectSkill )
		return false;

	if ( pSelectSkill->dwUseMethod & SkillUseMethod_SelectFirst )
		return true;

	return false;
}

bool IsCanSelectInHandSkill(SkillBase* pSelectSkill)
{
	if ( NULL == pSelectSkill )
		return false;

	if ( pSelectSkill->dwType == SkillType_Passive )
		return false;

	if ( pSelectSkill && ( (pSelectSkill->dwType == SkillType_State) || (pSelectSkill->dwType == SkillType_StateAttack) )
		&& (pSelectSkill->dwTarget & SkillTarget_Single)
		&& !( (pSelectSkill->dwTarget & SkillTarget_Self) && !(pSelectSkill->dwTarget & SkillTarget_Single) ) )

		return true;
	if ( pSelectSkill && (pSelectSkill->dwTarget & SkillTarget_Death) )
		return true;

	if ( pSelectSkill
		&& ( (pSelectSkill->dwTarget & SkillTarget_Pet)
			|| (pSelectSkill->dwTarget & SkillTarget_Summon) ) )		//Only for pet
		return true;
	
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \param dwLevel : 描述
 * \return 返回值的描述
 */
bool ReSetSkillData(SkillBase* pSkill,DWORD dwLevel)
{
	FUNCTION_BEGIN;

	if(pSkill->dwLevel == dwLevel) return true;
	SkillLevelBase_t* pSkillLevel = GetSkillLevelBase(pSkill->dwID,dwLevel);
	//Assert(pSkillLevel);
	if(!pSkillLevel) return false;
	
	//back up skill up item
	DWORD upItemID = pSkill->dwUpItemID;

	memcpy(pSkill,pSkillLevel,sizeof(SkillLevelBase_t));
	//reset up item id
	pSkill->dwUpItemID = upItemID;

#ifdef _AUTOPLAY
	if(gAutoGameConfig.fnReSetSkillData)
		gAutoGameConfig.fnReSetSkillData(pSkill,dwLevel);
#endif
	return true;

	FUNCTION_END;
}