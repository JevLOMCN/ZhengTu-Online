#include "Zebra.h"
#include "zMisc.h"
#include "SceneUser.h"
#include "Scene.h"
#include "TimeTick.h"
#include "ObjectManager.h"
#include "Chat.h"
#include "ScenesServer.h"
#include "SessionClient.h"
#include "Quest.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "QuestAction.h"
#include "script.h"
/**
 * \brief 构造函数
 */
zObject::zObject():zEntry()
{
	createtime=time(NULL);
	base=NULL;
	inserted=false;
	bzero(&data,sizeof(data));
	data.fivetype = FIVE_NONE;
	fill(data);
}

/**
 * \brief 生成对象ID
 */
void zObject::generateThisID()
{
	id=zMisc::randBetween(0,1)?zMisc::randBetween(-1000,0x80000000):zMisc::randBetween(1000,0x7FFFFFFE);
	data.qwThisID=id;
}

/**
 * \brief 析构从全局物品索引中删除自己，并清空相关属性
 */
zObject::~zObject()
{
	if(inserted)
	{
		goi->removeObject(this->id);
		inserted=false;
		createtime=0;
		base=NULL;
		bzero(&data,sizeof(data));
	}
}

void zObject::fill(t_Object& d)
{
	for (int i=0; i<5; ++i) 
	{
		_p1[i] = &d.str + i;
	}

	for(int i=0; i<16; ++i) 
	{
		_p2[i] = &d.pdam + i;
	}
	
	for(int i=0; i<2; ++i) 
	{
		_p2[i+16] = &d.atrating + i;
	}

}
// 增加特定道具类型银子购买绑定
void zObject::checkBind()
{
	switch(base->kind)
	{
		case ItemType_DoubleExp:        //52代表双倍经验类型
		case ItemType_Honor:            //53代表荣誉之星类型
		case ItemType_ClearProperty:    //59洗点道具
		case ItemType_KING:             //65国王令
		case ItemType_FAMILY:           //55家族令
		case ItemType_TONG:             //54帮主令
		case ItemType_Tianxia:         //73天下令
		case ItemType_Amulets:          //80
		case ItemType_Xunzhang:         // 勋章
		case ItemType_Hanbing:          //寒冰麒麟
		case ItemType_Chibang:         //翅膀
		case ItemType_Shenbing0:       // 金箍棒
		case ItemType_Shenbing1:       // 万剑阵
		case ItemType_Shenbing2:       // 王者之刃
		case ItemType_Shenshou1:       // 金月神兽
		case ItemType_Shenshou2:       // 白月神兽
		case ItemType_ShangfangBaojian:   // 尚方宝剑
		case ItemType_ZijinLongpao:       // 紫金龙袍
		case ItemType_Fabao:       // 法宝
		case ItemType_Fabaofj1:       // 法宝
		case ItemType_Fabaofj2:       // 法宝
		case ItemType_Fabaofj3:       // 法宝
		case ItemType_Fabaofj4:       // 法宝
		case ItemType_Fabaofj5:       // 法宝
		case ItemType_Fabaofj6:       // 法宝
		case ItemType_Fabaofj7:       // 法宝
		case ItemType_Fabaofj8:       // 法宝
		case ItemType_Fabaofj9:       // 法宝
		case ItemType_Fabaofj10:       // 法宝
		case ItemType_Fabaofj11:       // 法宝
		case ItemType_Fabaofj12:       // 法宝
		case ItemType_handbookitem1:       // 怪物图鉴
		case ItemType_handbookitem2:       // 怪物图鉴
		case ItemType_handbookitem3:       // 怪物图鉴
		case ItemType_handbookitem4:       // 怪物图鉴
		case ItemType_handbookitem5:       // 怪物图鉴
		case ItemType_handbookitem6:       // 怪物图鉴
		case ItemType_handbookitem7:       // 怪物图鉴
		case ItemType_handbookitem8:       // 怪物图鉴
		case ItemType_handbookitem9:       // 怪物图鉴
		case ItemType_handbookitem10:       // 怪物图鉴
		case ItemType_SEXX1:       // 十二星宿十二星宿
		case ItemType_SEXX2:       // 十二星宿
		case ItemType_SEXX3:       // 十二星宿
		case ItemType_SEXX4:       // 十二星宿
		case ItemType_SEXX5:       // 十二星宿
		case ItemType_SEXX6:       // 十二星宿
		case ItemType_SEXX7:       // 十二星宿
		case ItemType_SEXX8:       // 十二星宿
		case ItemType_SEXX9:       // 十二星宿
		case ItemType_SEXX10:       // 十二星宿
		case ItemType_SEXX11:       // 十二星宿
		case ItemType_SEXX12:       // 十二星宿
			{
				data.bind=1;
			}
			break;
		default:
			break;
	}
	//soke 新增的几个神之魔力等 自动补血 72 类钥匙类购买绑定
	if(base->id == 881 || base->id == 882 || base->id == 929 || base->id == 930 || base->id == 942 || base->id == 952 || base->id == 953)
	{
		data.bind=1;
	}
	// 恋恋全部绑定
	// data.bind=1;
}

//soke 增加特定道具类型金子购买绑定
void zObject::checkXinBind()
{
	switch(base->kind)
	{
		//case ItemType_Scrxll:	        //15代表新物品类
		//case ItemType_Resource:	        //16代表原料类
		//case ItemType_LevelUp:          //27代表道具升级需要的材料类 如各种升级宝石
		case ItemType_Gift:	            //31代表礼品类物品.
	    case ItemType_Other:            //32右键使用召唤NPC
		case ItemType_Quest:            //34右键使用任务物品
		case ItemType_Xuest:            //49类型物品
		case ItemType_DoubleExp:        //52代表双倍经验类型
		case ItemType_Honor:            //53代表荣誉之星类型
		case ItemType_GreatLeechdom:    //58大计量自动补血道具
		case ItemType_ClearProperty:    //59洗点道具
		case ItemType_GreatLeechdomMp:  //62大计量自动补蓝道具
		case ItemType_Tianxia:  //73天下令
		case ItemType_KING:             //65国王令
		case ItemType_FAMILY:           //55家族令
		case ItemType_TONG:             //54帮主令
		case ItemType_Resource:             //16材料
		case ItemType_Leechdom:             //17药水
		case ItemType_GreatLeechdomHp:  //72大计量自动补血道具
		case ItemType_FashionBody:		//119代表时装
		case ItemType_HighFashionBody:  //124代表高级时装
		case ItemType_XHighFashionBody: //125代表新高级时装
		case ItemType_SHighFashionBody: //126代表新高级透明时装
		case ItemType_ShangfangBaojian:   // 尚方宝剑
		case ItemType_ZijinLongpao:       // 紫金龙袍
		case ItemType_Fabao:       // 法宝
		case ItemType_Fabaofj1:       // 法宝
		case ItemType_Fabaofj2:       // 法宝
		case ItemType_Fabaofj3:       // 法宝
		case ItemType_Fabaofj4:       // 法宝
		case ItemType_Fabaofj5:       // 法宝
		case ItemType_Fabaofj6:       // 法宝
		case ItemType_Fabaofj7:       // 法宝
		case ItemType_Fabaofj8:       // 法宝
		case ItemType_Fabaofj9:       // 法宝
		case ItemType_Fabaofj10:       // 法宝
		case ItemType_Fabaofj11:       // 法宝
		case ItemType_Fabaofj12:       // 法宝
		case ItemType_handbookitem1:       // 怪物图鉴
		case ItemType_handbookitem2:       // 怪物图鉴
		case ItemType_handbookitem3:       // 怪物图鉴
		case ItemType_handbookitem4:       // 怪物图鉴
		case ItemType_handbookitem5:       // 怪物图鉴
		case ItemType_handbookitem6:       // 怪物图鉴
		case ItemType_handbookitem7:       // 怪物图鉴
		case ItemType_handbookitem8:       // 怪物图鉴
		case ItemType_handbookitem9:       // 怪物图鉴
		case ItemType_handbookitem10:       // 怪物图鉴	
		case ItemType_SEXX1:       // 十二星宿十二星宿
		case ItemType_SEXX2:       // 十二星宿
		case ItemType_SEXX3:       // 十二星宿
		case ItemType_SEXX4:       // 十二星宿
		case ItemType_SEXX5:       // 十二星宿
		case ItemType_SEXX6:       // 十二星宿
		case ItemType_SEXX7:       // 十二星宿
		case ItemType_SEXX8:       // 十二星宿
		case ItemType_SEXX9:       // 十二星宿
		case ItemType_SEXX10:       // 十二星宿
		case ItemType_SEXX11:       // 十二星宿
		case ItemType_SEXX12:       // 十二星宿	
			{
				data.bind=1;
			}
			break;
		default:
			break;
	}
	//soke 魂魄石掉落绑定和非绑定 金子购买
	
	if(base->id == 3540 || base->id == 3541 || base->id == 3542 || base->id == 3543 || base->id == 3544 || base->id == 942 || base->id == 952 || base->id == 953)
	{
		data.bind=1;
	}

	//恋恋全部绑定
	//data.bind=1;
}
///////////////////////////////////////////////////////////
DWORD zObject::RepairMoney2RepairGold(DWORD dwObjectRepair)
{
	return (DWORD)((float)(dwObjectRepair/50)+0.99);
}
/**
 * \brief 物品log
 * \param createid 	物品创建id
 * \param objid 	物品Thisid
 * \param objname 	物品名称
 * \param num 		物品数量
 * \param change	物品数量
 * \param type	 	变化类型(2表示上线加载,1表示增,0表示减)
 * \param srcid 	源id
 * \param srcname  	源名称
 * \param dstid 	目的id
 * \param dstname 	目的名称
 * \param action 	操作
 *
void zObject::logger(QWORD createid,DWORD objid,char *objname,DWORD num,DWORD change,DWORD type,DWORD srcid,char *srcname,DWORD dstid,char *dstname,const char *action)
{
	*
	char _objname[MAX_NAMESIZE + 1];
	char _srcname[MAX_NAMESIZE + 1];
	char _dstname[MAX_NAMESIZE + 1];
	char _action[MAX_NAMESIZE + 1];
	bzero(_objname,sizeof(_objname));
	bzero(_srcname,sizeof(_srcname));
	bzero(_dstname,sizeof(_dstname));
	bzero(_action,sizeof(_action));
	if(objname)
	{
		strncpy(_objname,objname,MAX_NAMESIZE +1);
	}
	else
	{
		strncpy(_objname,"NULL",MAX_NAMESIZE +1);
	}
	if(srcname)
	{
		strncpy(_srcname,srcname,MAX_NAMESIZE +1);
	}
	else
	{
		strncpy(_srcname,"NULL",MAX_NAMESIZE +1);
	}
	if(dstname)
	{
		strncpy(_dstname,dstname,MAX_NAMESIZE +1);
	}
	else
	{
		strncpy(_dstname,"NULL",MAX_NAMESIZE +1);
	}
	if(action)
	{
		strncpy(_action,action,MAX_NAMESIZE +1);
	}
	else
	{
		strncpy(_action,"NULL",MAX_NAMESIZE +1);
	}
	// /
	ScenesService::objlogger->debug("%llu,%u,%s,%u,%u,%u,%u,%s,%u,%s,%s",createid,objid,objname,num,change,type,srcid,srcname,dstid,dstname,action);
}*/
/**
 * \brief 物品log
 * \param createid      物品创建id
 * \param objid         物品Thisid
 * \param objname       物品名称
 * \param num           物品数量
 * \param change        物品数量
 * \param type          变化类型(2表示上线加载,1表示增,0表示减)
 * \param srcid         源id
 * \param srcname       源名称
 * \param dstid         目的id
 * \param dstname       目的名称
 * \param action        操作
 * \param base          物品基本表里的指针
 * \param kind          物品的种类
 * \param upgrade       物品的等级（升级的次数）
 * \brief		其中后三个参数是用来打印，追加的日志的，包括装备的颜色，星级，材料的等级，宝石的等级
 */
void zObject::logger(QWORD createid,DWORD objid,char *objname,DWORD num,DWORD change,DWORD type,DWORD srcid,char *srcname,DWORD dstid,char *dstname,const char *action, zObjectB *base,BYTE kind,BYTE upgrade)
{
        char tmpInfo[60] = {0};
        char *p = tmpInfo;

        if(srcname != NULL)
                strcat(tmpInfo, srcname);

	if(base != NULL)
	{
        	switch (base->kind)
        	{
                	case ItemType_ClothBody:                //101代表布质加生命类服装
                	case ItemType_FellBody:                 //102代表皮甲加魔防类服装
                	case ItemType_MetalBody:                //103代表金属铠甲加物防类服装
                	case ItemType_Blade:                    //104代表武术刀类武器
                	case ItemType_Sword :                   //105代表武术剑类武器
               		case ItemType_Axe:                      //106代表武术斧类武器
                	case ItemType_Hammer:                   //107代表武术斧类武器
                	case ItemType_Staff:                    //108代表法术杖类武器
                	case ItemType_Crossbow:                 //109代表箭术弓类武器
                	case ItemType_Fan:                      //110代表美女扇类
                	case ItemType_Stick:                    //111代表召唤棍类武器
                	case ItemType_Shield:                   //112代表盾牌类
                	case ItemType_Helm:                     //113代表角色头盔类
                	case ItemType_Caestus:                  //114代表角色腰带类
                	case ItemType_Cuff:                     //115代表角色护腕类
                	case ItemType_Shoes:                    //116代表角色鞋子类
                	case ItemType_Necklace:                 //117代表角色项链类
                	case ItemType_Fing:                     //118代表角色戒指类
					case ItemType_HorseShoe:                //130代表坐骑马脚类
					case ItemType_HorseRope:                //131代表坐骑马绳类
					case ItemType_HorseSaddle:              //132代表坐骑马鞍类
					case ItemType_HorseSafe:                //133代表坐骑马甲类
					case ItemType_HorseIron:                //134代表坐骑马镫类
					case ItemType_Dagger:	                //136代表利刃
					case ItemType_NewSword:	                //137代表卫士剑
	        		case ItemType_breastplate:	            //138代表护心镜
	        		case ItemType_StaffFlag:		        //141代表战旗(法师)
	        		case ItemType_CrossFlag:	            //142代表战旗(弓手)
	        		case ItemType_Flag:	                    //143代表战旗(仙术)
	        		case ItemType_StickFlag:	            //144代表战旗(召唤)
	        		case ItemType_BladeFlag:	            //145代表战旗(战士)
	        		case ItemType_DaggerFlag:	            //146代表战旗(刺客)
	        		case ItemType_SwordFlag:                //147代表战旗(卫士)
	        		case ItemType_LongGun:                  //155代表长枪
	        		case ItemType_Handgun:                  //156代表短枪
                        	{
                                	switch(kind)
                                	{
                                        	case 0:
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "白色:");
                                                	break;
                                        	case 1:
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "蓝色:");
                                                	break;
                                        	case 2:
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "黄色:");
                                                	break;
                                        	case 4:
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "绿色:");
                                                	break;
								            ///////////////////////////////////////////
											case 16:
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "紫色:");
                                                	break;
											///////////////////////////////////////////		
                                        	default:
                                               		break;
                                	}
                                	sprintf(p + strlen(tmpInfo), "%d",upgrade);
                        	}
                        	break;

                	case ItemType_Resource: //16代表原料类 
                        	{
                                	switch(base->id)
                                	{
                                        	//下面是需要显示等级的物品ID
                                        	case 506:
                                        	case 507:
                                        	case 516:
                                        	case 517:
                                        	case 526:
                                        	case 527:
                                        	case 536:
                                        	case 537:
                                        	case 546:
                                        	case 547:
											case 548:
											case 550:
											case 552:
											case 561:
											case 562:
											case 563:
											case 564:
											case 565:
											case 852:
                                        	case 877:
											case 1507:
											case 1517:
											case 1527:
											case 1537:
											case 1547:
                                        	{
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "材料:");
                                                	sprintf(p + strlen(tmpInfo), "%d",(upgrade+1));

                                        	}
                                        	default:
                                               	 	break;

                                	}
                        	}
                        	break;
                	case ItemType_LevelUp:       //27代表道具升级需要的材料类
                        	{
                                	switch(base->id)
                                	{
                                        	case 678:
                                        	case 679:
                                        	{
                                                	strcat(p + strlen(tmpInfo), ":");
                                                	strcat(tmpInfo, "宝石:");
                                                	sprintf(p + strlen(tmpInfo), "%d",(upgrade+1));
                                        	}
                                        	default:
                                                	break;
                                	}
                        	}
                        	break;
                	default:
                        	break;
        	}
	}

        ScenesService::objlogger->debug("%llu,%u,%s,%u,%u,%u,%u,%s,%u,%s,%s",createid,objid,objname,num,change,type,srcid,tmpInfo,dstid,dstname,action);
}
/**
 * \brief 根据物品对象复制一个新的物品对象
 * \param objsrc 参照对象
 * \return 失败返回NULL 否则返回生成的对象
 */
zObject *zObject::create(zObject *objsrc)
{
	if(objsrc==NULL) return NULL;
	zObject *ret=new zObject();
	if(ret)
	{
		strncpy(ret->name,objsrc->name,MAX_NAMESIZE);
		ret->tempid=objsrc->id;
		ret->base=objsrc->base;
		bcopy(&objsrc->data,&ret->data,sizeof(ret->data));
		ret->generateThisID();
		ret->free(true);

		if(!goi->addObject(ret))
		{
			SAFE_DELETE(ret);
		}
		else
		{
			ret->dwCreateThisID=ret->data.qwThisID;
			ret->inserted=true;
		}
	}
	return ret;
}

void  zObject::destroy(zObject* ob)
{
	assert(!ob || ob->free());
	SAFE_DELETE(ob);
}

const stObjectLocation &zObject::reserve() const
{
	return data.pos;
}

void zObject::restore(const stObjectLocation &loc)
{
	data.pos = loc;
}

bool zObject::free() const
{
	return data.pos == Zt_Object::INVALID_POS || data.pos.loc() == Cmd::OBJECTCELLTYPE_NONE;
}

void zObject::free(bool flag)
{
	data.pos = Zt_Object::INVALID_POS;
}

//soke 根据物品TBL的配置生成实际物品对象
/**
 * \brief 根据物品字典创建一个物品对象
 * \param objbase 物品字典
 * \param num 物品的数量
 * \param level 物品的级别
 * \return 物品对象
 */
zObject *zObject::create(zObjectB *objbase, unsigned int num, BYTE level)
{
	if(objbase==NULL) return NULL;

	//only money can have 0
	if ( (num == 0 && objbase->id != 665) || num > objbase->maxnum) 
	{
		return NULL;
	}
	
	zObject *ret=new zObject();
	if(ret)
	{
		ret->base = objbase;
		ret->generateThisID();
		strncpy(ret->name, objbase->name, MAX_NAMESIZE);
		strncpy(ret->data.strName, objbase->name, MAX_NAMESIZE);
/*		
		ret->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_COMMON;
*/
		ret->data.dwObjectID = objbase->id;

		ret->free(true);
		ret->tempid = objbase->id;
		ret->data.dwNum = num;
		ret->data.color = objbase->color;
		switch (objbase->kind)
		{
			case ItemType_ClothBody:		//101代表布质加生命类服装
			case ItemType_FellBody:		    //102代表皮甲加魔防类服装
			case ItemType_MetalBody:		//103代表金属铠甲加物防类服装
			case ItemType_Blade:		    //104代表武术刀类武器
			case ItemType_Sword :	        //105代表武术剑类武器
			case ItemType_Axe:	           //106代表武术斧类武器
			case ItemType_Hammer:	        //107代表武术斧类武器
			case ItemType_Staff:		    //108代表法术杖类武器
			case ItemType_Crossbow:	        //109代表箭术弓类武器
			case ItemType_Fan:	           //110代表美女扇类
			case ItemType_Stick:	        //111代表召唤棍类武器
			case ItemType_Shield:	        //112代表盾牌类
			case ItemType_Helm:		        //113代表角色头盔类
			case ItemType_Caestus:	        //114代表角色腰带类
			case ItemType_Cuff:		        //115代表角色护腕类
			case ItemType_Shoes:		    //116代表角色鞋子类
			case ItemType_Necklace:	        //117代表角色项链类
			case ItemType_Fing:		        //118代表角色戒指类
			case ItemType_HorseShoe:        //130代表坐骑马脚类
			case ItemType_HorseRope:        //131代表坐骑马绳类
			case ItemType_HorseSaddle:      //132代表坐骑马鞍类
			case ItemType_HorseSafe:        //133代表坐骑马甲类
			case ItemType_HorseIron:        //134代表坐骑马镫类
			case ItemType_HorseFashion:     //140代表坐骑马铃铛
			case ItemType_Dagger:	        //136代表利刃
			case ItemType_NewSword:	        //137代表卫士剑
	        case ItemType_breastplate:	    //138代表护心镜
	        case ItemType_StaffFlag:		//141代表战旗(法师)
	        case ItemType_CrossFlag:	    //142代表战旗(弓手)
	        case ItemType_Flag:	            //143代表战旗(仙术)
	        case ItemType_StickFlag:	    //144代表战旗(召唤)
	        case ItemType_BladeFlag:	    //145代表战旗(战士)
	        case ItemType_DaggerFlag:	    //146代表战旗(刺客)
	        case ItemType_SwordFlag:        //147代表战旗(卫士)
	        case ItemType_LongGun:          //155代表长枪
	        case ItemType_Handgun:          //156代表短枪
			case ItemType_FashionBody:		//119代表时装
					ret->data.color  = zMisc::randBetween(0xFF000000, 0xFFFFFFFF);
					break;		
		    case ItemType_HighFashionBody:  //124代表高级时装
			   ret->data.color = zMisc::randBetween(0xFF000000,0xFFFFFFFF);
			   break;
		   case ItemType_XHighFashionBody:  //125代表新高级时装
			   ret->data.color = zMisc::randBetween(0xFF000000,0xFFFFFFFF);
			   break;
			case ItemType_SHighFashionBody:  //126代表新高级透明时装
			   ret->data.color = zMisc::randBetween(0xFF000000,0xFFFFFFFF);
			   break;  
		}
		
		ret->data.needlevel = objbase->needlevel;				// 需要等级
	
		ret->data.maxhp = objbase->maxhp;					// 最大生命值
		ret->data.maxmp = objbase->maxmp;					// 最大法术值
		ret->data.maxsp = objbase->maxsp;					// 最大体力值
	
		ret->data.pdamage = objbase->pdamage;				// 最小攻击力
		ret->data.maxpdamage = objbase->maxpdamage;			// 最大攻击力
		ret->data.mdamage = objbase->mdamage;				// 最小法术攻击力
		ret->data.maxmdamage = objbase->maxmdamage;			// 最大法术攻击力
	
		ret->data.pdefence = objbase->pdefence;				// 物防
		ret->data.mdefence = objbase->mdefence;				// 魔防
		ret->data.damagebonus = objbase->damagebonus;			// 伤害加成
		
		ret->data.akspeed = objbase->akspeed;				// 攻击速度
		ret->data.mvspeed = objbase->mvspeed;				// 移动速度
		ret->data.atrating = objbase->atrating;				// 命中率
		ret->data.akdodge = objbase->akdodge;				// 躲避率
		ret->data.bang = objbase->bang;
		ret->data.dur = objbase->durability;
		ret->data.maxdur = objbase->durability;
		ret->data.price = objbase->price;
		ret->data.cardpoint = objbase->cardpoint;
		
		ret->data.upgrade = level;								// 初始等级

		if(!goi->addObject(ret))
		{
			SAFE_DELETE(ret);
		}
		else
		{
			ret->dwCreateThisID=ret->data.qwThisID;
			ret->inserted = true;
			//soke 取消场景日志显示
           // Zebra::logger->debug("创建物品%s成功等级%d", objbase->name,ret->data.upgrade);
           // ret->data.upgrade = level;
		}
	}else {
		Zebra::logger->debug("创建物品%s失败", objbase->name);
	}
	
	return ret;
}

/**
 * \brief 从档案服务器读物品
 *
 * \param o 从档案服务器中读到的物品
 *
 * \return load成功返回该物品,否则返回NULL
 */
zObject *zObject::load(const SaveObject *o)
{
	if(o==NULL) return NULL;
	zObjectB *objbase = objectbm.get(o->object.dwObjectID);
	if(objbase==NULL) 
	{
		Zebra::logger->debug("加载物品失败,道具基本表中不存在:%d",o->object.dwObjectID);
		return NULL;
	}
	int i=0;
	zObject *ret=NULL; 
	while(!ret && i < 100)
	{
		ret=new zObject();
		if( i > 1)
		{
			Zebra::logger->debug("尝试new出新的物品对象次数:%d",i);
		}
		i ++;
	}
	if(ret == NULL) 
	{
		Zebra::logger->debug("加载物品失败,new物品对象失败:%d",o->object.dwObjectID);
		return ret; 
	}
	bcopy(&o->object , &ret->data , sizeof(t_Object));
	ret->createid = o->createid;
	ret->id = ret->data.qwThisID;
	ret->tempid = ret->data.dwObjectID;
	strncpy(ret->name,ret->data.strName,MAX_NAMESIZE);
	ret->base=objbase;

	if(!goi->addObject(ret))
	{
		SAFE_DELETE(ret);
	}
	else
		ret->inserted=true;
	return ret;
}
/**
 * \brief 得到物品创建时间,存档时使用
 *
 * \return 物品创建时间
 */
bool zObject::getSaveData(SaveObject *save)
{
	bcopy(&data , &save->object , sizeof(t_Object));
	save->createid =  createid;
	return true;
}

/**
 * \brief 返回物品孔的数目

 * \return 孔的数目
 */
int zObject::foundSocket()
{
	WORD socket = 0;
	if (zMisc::selectByPercent(base->hole.odds)) {
		socket = zMisc::randBetween(base->hole.min, base->hole.max);
		for(int i=0; i<socket; ++i)
			data.socket[i] = (DWORD)-1;
	}
	
	return socket;
}

/**
 * \brief 返回物品是否可以邮寄

 * \return 是否可以
 */
bool zObject::canMail()
{
	if (data.bind || data.dwObjectID == 800 || base->kind == ItemType_Quest || data.dwObjectID == 734 )
		return false;
	if (data.pos.loc() !=Cmd::OBJECTCELLTYPE_COMMON 
			&& data.pos.loc() !=Cmd::OBJECTCELLTYPE_PACKAGE
			&& data.pos.loc() !=Cmd::OBJECTCELLTYPE_PET)
		return false;
	return true;
}

/**
 * \brief 返回物品是否可以被捐献

 * \return 是否可以
 */
zCountryMaterialB* zObject::canContribute()
{
	zCountryMaterialB* country_material = NULL;
	country_material = countrymaterialbm.get(data.dwObjectID+base->kind);
	return country_material;
}

/**
 * \brief 返回物品原料类别

 * \return 类别
 */
DWORD zObject::getMaterialKind()
{
	DWORD ret = 0;
	
	if (base->kind == ItemType_Resource)
	{
		if (data.dwObjectID == 501 || data.dwObjectID == 502 || data.dwObjectID == 506 || data.dwObjectID == 507)
		{
			ret = 1;
		}
		else if (data.dwObjectID == 511 || data.dwObjectID == 512 
			|| data.dwObjectID == 516 || data.dwObjectID == 517)
		{
			ret = 2;
		}
		else if (data.dwObjectID == 521 || data.dwObjectID == 522 
			|| data.dwObjectID == 526 || data.dwObjectID == 527)
		{
			ret = 3;
		}
		else if (data.dwObjectID == 531 || data.dwObjectID == 532 
			|| data.dwObjectID == 536 || data.dwObjectID == 537)
		{
			ret = 4;
		}
		else if (data.dwObjectID == 541 || data.dwObjectID == 542
			|| data.dwObjectID == 546 || data.dwObjectID == 547)
		{
			ret = 5;
		}
		else if (data.dwObjectID == 554 || data.dwObjectID == 555
			|| data.dwObjectID == 559 || data.dwObjectID == 560)
		{
			ret = 6;
		}
	}

	return ret;
}


/**
 * \brief 包裹构造
 */
Package::Package(WORD type,DWORD id, WORD w, WORD h):_type(type),_id(id), _width(w), _height(h), _space(w*h), _size(_space)

{
	WORD cap = _size;
	if (cap == 0) cap = 1;
	container = new zObject* [cap];
	memset(container, 0, sizeof(zObject*)*cap);
}

Package::~Package()
{
	removeAll();
	SAFE_DELETE_VEC(container);
}

bool Package::checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y)
{
	if (object==NULL) return true;
	zObject *temp;
	return getObjectByZone(&temp, x, y);
}

bool Package::add(zObject* object, bool find)
{
	if(!object  || object->base->kind==ItemType_Money)
		return false;
    //find 主动寻找一个空格子
	if(find)	{
		if(find_space(object->data.pos.x, object->data.pos.y))	{
			return add(object,false);
		}else {
			//Zebra::logger->warn("包裹[%d:%d]中找不到空间存放物品[%x]", _type, _id, object);
			return false;
		}	
	}

	if(!find)	{
		int pos = position(object->data.pos.xpos(), object->data.pos.ypos());
		if (pos == -1 || pos >= size()) {
			Zebra::logger->warn("包裹[%d:%d]中添加物品[%x]时索引[%d]错误", _type, _id, object, pos);
			return false;
		}
		if (container[pos]) { 
			//shouldn't be reached at all
			Zebra::logger->warn("包裹[%d]中[%d, %d]已有物品%x，不能存放物品%x", _id, object->data.pos.x, object->data.pos.y, container[pos], object);
			return false;
		}else {

		//	assert(!container[pos]);
			container[pos] = object;
			--_space;
			return true;
		}
	}

	//never reach
	return false;

}

bool Package::find_space(WORD &x,WORD &y) const
{
	for (int i=0; i<_width; ++i)
		for(int j=0; j<_height; ++j) 
			if (!container[j*_width+i]) {
				x = i;
				y = j;
				return true;
			}

	return false;
}

int Package::position(WORD x, WORD y) const
{
	return y*_width + x;
}


bool Package::remove(zObject *object)
{
	if(object)
	{
		int pos = position(object->data.pos.xpos(), object->data.pos.ypos());
		if (pos == -1 || pos >= size() || container[pos] != object) {
			Zebra::logger->warn("包裹[%d:%d]中删除物品[%x]时索引[%d]错误", _type, _id, object, pos);
			return false;
		}

		object->free(true);
		container[pos] = NULL;
		++_space;
		return true;
	}

	return false;
}

void Package::removeAll()
{
	for (int i=0; i<size(); ++i) {
		SAFE_DELETE(container[i]);
	};
}

// 定义自定义异常类
#include <stdexcept> // 引入标准异常类
#include <string>    // 引入字符串类

class ObjectNotFoundException : public std::runtime_error {
	public:
		ObjectNotFoundException(const std::string& message)
			: std::runtime_error(message) {}
	};
	
class ArrayIndexOutOfBoundsException : public std::runtime_error {
	public:
		ArrayIndexOutOfBoundsException(const std::string& message)
			: std::runtime_error(message) {}
	};

bool Package::getObjectByZone(zObject **ret, WORD x, WORD y)
{
    int pos = position(x, y);
    if (pos >= size() || pos == -1) {
        Zebra::logger->debug("=================================包裹获取物品时索引[%d]超出范围", pos);
        return false;
    }
    
    // 异常处理
    try {
        *ret = container[pos];
    }
    catch (const ObjectNotFoundException &e) {
        Zebra::logger->debug("=================================包裹获取物品B[%d, %d]时索引[%d]错误：%s",  x, y, pos, e.what());
        return false;
    }
    catch (const ArrayIndexOutOfBoundsException &e) {
        Zebra::logger->debug("=================================包裹获取物品C[%d, %d]时索引[%d]错误：%s",  x, y, pos, e.what());
        return false;
    }
    catch (const std::runtime_error &e) {
        Zebra::logger->debug("=================================包裹获取物品D[%d, %d]时索引[%d]错误：%s",  x, y, pos, e.what());
        return false;
    }
    catch (const std::exception &e) {
        Zebra::logger->debug("=================================包裹获取物品A[%d, %d]时索引[%d]错误：%s",  x, y, pos, e.what());
        return false;
    }
    return true;
}


bool Package::getObjectByID(zObject **ret, DWORD id)
{
	for (int i=0; i<size(); ++i)
	{
		if (!container[i]) continue;

		if (container[i]->base->id==id)
		{
			*ret = container[i];
			return true;
		}
	}
	return false;
}

void Package::execEvery(PackageCallback &callback)
{
	for (int i=0; i<size(); ++i) {
		if (!container[i]) continue;
		if (!callback.exec(container[i])) break;
	}
}

WORD Package::space() const
{
	return _space;
}

WORD Package::size() const
{
	return _size;
}

/**
 * \brief 获取包裹类型
 * \return 包裹类型
 */
WORD Package::type() const
{
	return _type;
}

/**
 * \brief 获取包裹ID
 * \return 包裹ID
 */
DWORD Package::id() const
{
	return _id;
}

void Package::setSpace(WORD s)
{
	_space = s;
}
#if 0
/**
 * \brief 多格包
 * \param type 类型
 * \param id 编号
 * \param width 宽度
 * \param height 高度
 */
MultiPack::MultiPack(WORD type,DWORD id ,WORD width,WORD height):Package(type,id)
{
	this->width=width;
	this->height=height;
	grid.resize(height);
	for(int i = 0; i < height; i++)
	{
		grid[i].resize(width);
		for(int j=0;j<width;j++)
			grid[i][j]=NULL;
	}
}

/**
 * \brief 析构函数
 */
MultiPack::~MultiPack()
{
	for(std::set<zObject *>::iterator it=allset.begin();it!=allset.end();it++)
	{
		zObject *o = *it;
		SAFE_DELETE(o);
	}
	allset.clear();
	for(int i = 0; i < height; i++)
		for(int j=0;j<width;j++)
			grid[i][j]=NULL;
}

/**
 * \brief 添加装备
 * \param object 物品对象
 * \param find 是否自动寻找位置
 * \return 添加成功返回true 否则返回false
 */
bool MultiPack::add(zObject *object,bool find)
{ 
	if(object==NULL || object->base->kind==ItemType_Money)
		return false;
	if(find)
	{
		if(findSpace(object->base->width,object->base->height,object->data.pos.x,object->data.pos.y))
		{
			return add(object,false);
		}
	}
	else if(object && object->data.pos.x+object->base->width<=width  && object->data.pos.y+object->base->height <=height)
	{
		object->data.pos.dwLocation=type;
		object->data.pos.dwTableID=id;
		WORD i = 0;
		WORD j = 0;
		for(i=object->data.pos.x;i<object->data.pos.x + object->base->width;i++)
			for(j=object->data.pos.y;j<object->data.pos.y + object->base->height;j++) 
				grid[j][i]=object;
		allset.insert(object); 
		/*
		if(!allset.insert(object).second && object->base->width == 1 && object->base->height == 1)
		{
			Zebra::logger->debug("重复加入物品");
			grid[j][i]=0;
		}
		// */
		/*
		for(WORD j=0;j<height;j++)
		{
			printf("\n");
			for(WORD i=0;i<width;i++)
				if(grid[j][i]==NULL)
					printf("0");
				else
					printf("*");
		}
		printf("\n");
		// */
		return true;
	}
	return false;
}

/**
 * \brief 检查物品
 * \param pUser 角色
 * \param object 物品对象
 * \param x 横坐标
 * \param y 纵坐标
 * \return true 装备有此物品
 */
bool MultiPack::checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y)
{
	if (object==NULL) return true;
	zObject *temp;
	return getObjectByZone(&temp, x, y,object->base-> width,object->base-> height);
}

/**
 * \brief 从包裹里删除物品
 * \param object 要处理的对象
 */
void MultiPack::remove(zObject *object)
{
	if(object)
	{
		object->data.pos.dwLocation=Cmd::OBJECTCELLTYPE_NONE;
		object->data.pos.dwTableID=0;
		allset.erase(object);
		int count=0;
		for(WORD i=0;i<width;i++)
			for(WORD j=0;j<height;j++) {
				if(grid[j][i]==object)
				{
					count++;
					grid[j][i]=NULL;
				}
			}
		if (count>1) {
			Zebra::logger->fatal("物品(%s)坐标(%d,%d)在主包裹中有镜像", object->name, object->data.pos.x, object->data.pos.y);
			return;
		}
		/*
				if (i>=width || j >=height) {
					Zebra::logger->fatal("物品(%s)坐标错误(%d,d)", object->name, object->data.pos.x, object->data.pos.y);
					return;
				}
		for(WORD i=object->data.pos.x;i<object->data.pos.x+object->base->width;i++)
			for(WORD j=object->data.pos.y;j<object->data.pos.y+object->base->height;j++) {
				if (i>=width || j >=height) {
					Zebra::logger->fatal("物品(%s)坐标错误(%d,d)", object->name, object->data.pos.x, object->data.pos.y);
					return;
				}
				if(grid[j][i]==object) grid[j][i]=NULL;
			}
			*/
	}
}

/**
 * \brief 清空包裹
 * \param object 要处理的对象
 */
void MultiPack::removeAll()
{
	for (std::set<zObject *>::iterator it=allset.begin(); it!=allset.end(); ++it) 
	{
		remove(*it);
	}
	allset.clear();
}

/**
 * \brief  根据位置拿到对象
 * \param ret 回传拿到的对象
 * \param x   横坐标
 * \param y   纵坐标
 * \param w   宽度
 * \param h   高度
 * \return true 得到物品， false 该区域很复杂不只一个物品
 */
bool MultiPack::getObjectByZone(zObject **ret,WORD x,WORD y,WORD w,WORD h)
{
	*ret=NULL;
	if(x+w>width || y+h>height) return false;
	int count=0;
	for(WORD i=x;i<x+w;i++)
		for(WORD j=y;j<y+h;j++)
			if(grid[j][i]!=*ret && grid[j][i]!=NULL)
			{
				count++;
				*ret=grid[j][i];
			}
	return (count <=1);
}

/**
 * \brief 查找背包空间
 * \param owidth 物品宽
 * \param oheight 物品高
 * \param 查找到的x坐标
 * \param 查找到的y坐标
 * \return true 找到合适的位置，false 没找到适合的位置
 */
bool MultiPack::findSpace(WORD owidth,WORD oheight,WORD &x,WORD &y)
{
		bool finded=false;
		WORD i=0,j=0;
		for(i=0;i<=width-owidth;i++)
		{
			for(j=0;j<=height-oheight;j++)
			{
				bool space=true;
				for(WORD w=0;w<owidth;w++)
				{
					for(WORD h=0;h<oheight;h++)
						if(grid[j+h][i+w]!=NULL)
						{
							space=false;
							break;
						}
					if(!space) break;
				}
				if(space)
				{
					finded=true;
					break;
				}
			}
			if(finded) break;
		}

		if(finded)
		{
			x=i;
			y=j;
		}
		return finded;
}

/**
 * \brief 遍历，空方法
 * \param callback 回调
 */
void MultiPack::execEvery(PackageCallback &callback)
{
	for(std::set<zObject *>::iterator it=allset.begin(); it!=allset.end(); ++it) {
		if (!callback.exec(*it)) break;
	}
}
#endif

/**
 * \brief 装备包裹的构造
 * \param user 包裹的主人
 */
EquipPack::EquipPack(SceneUser* user):Package(Cmd::OBJECTCELLTYPE_EQUIP,0, 1, Cmd::EQUIPCELLTYPE_MAX) , owner(user)
{
	needRecalc=true;
	packs = new ObjectPack*[4];
	memset(packs, 0, 4*sizeof(ObjectPack*));
	doubleexp_obj=0; 
	doubleexp_obj_time=0; 
	family_obj_times=5; //soke 家族令使用次数
	family_obj_time=0;
	tong_obj_times=2;
	tong_obj_time=0;
	king_obj_times=2;
	king_obj_time=0;
	effectCount=0;
	effecthanbing=0;
	effectangel=0;
	effect1star=0;
	effect2star=0;
	effect3star=0;
	effect5star=0;
	effect8star=0;
	effect10star=0;
	effect12star=0;
	effect13star=0;
	effect14star=0;
	effect15star=0;
    // 等级套装
	effect16star=0; //桃子 乾坤套装
	effect17star=0; //桃子 炙天套装
	effect18star=0; //桃子 轩辕套装
	effect19star=0; //桃子 卓越套装
	effect20star=0; //桃子 传说套装
	effect21star=0; //桃子 天尊套装
	effect22star=0; //桃子 英雄套装
	effect23star=0; //桃子 圣者套装
	effect24star=0; //桃子 战神套装
	effect25star=0; //桃子 帝王套装
	effect26star=0; //桃子 弑天套装
	effect27star=0; //桃子 永恒套装
    // 图鉴套装
	effecttj1star=0; //桃子 白图鉴套装
	effecttj2star=0; //桃子 蓝图鉴套装
	effecttj3star=0; //桃子 黄图鉴套装
	effecttj4star=0; //桃子 绿图鉴套装
	effecttj5star=0; //桃子 紫图鉴套装
    effecttj6star=0; //桃子 橙图鉴套装
	effecttj7star=0; //桃子 紫图鉴套装

	effect21linghun=0;
	effect22linghun=0;
	effect23linghun=0;
	effect24linghun=0;
	effect25linghun=0;
	effect5mapai=0;
	effect8mapai=0;
	effect10mapai=0;
	effect12mapai=0;
	effect13mapai=0;

	// myy 斗魂套装效果
	effect3douhun=0;  //斗魂3星套
	effect6douhun=0;  //斗魂6星套
	effect9douhun=0;  //斗魂9星套
	effect12douhun=0;  //斗魂12星套
	effect15douhun=0;  //斗魂15星套

	effect5longx=0;
	effect8longx=0;
	effect11longx=0;
	effect13longx=0;
	effect15longx=0;

	effect1butian=0;
	effect2butian=0;
	effect3butian=0;
	effect4butian=0;
	effect5butian=0;

	effect2zhuangbei=0;
	effect3zhuangbei=0;
	effect4zhuangbei=0;
	effect5zhuangbei=0;
	effect6zhuangbei=0;

	effect1shenqi=0;
	effect2shenqi=0;
	effect3shenqi=0;
	effect4shenqi=0;
	effect5shenqi=0;
	effect6shenqi=0;
	effect7shenqi=0;
}

/**
 * \brief 析构函数
 */
EquipPack::~EquipPack()
{
	for (int i=0; i<4; ++i) 
	{
		SAFE_DELETE(packs[i]);
	}
	SAFE_DELETE_VEC(packs);
}

bool EquipPack::isTonic()
{
	if(container[13] && container[13]->base->kind == ItemType_Tonic)
	{
		return true;
	}
	return false;
}
/**
 * \brief 更新装备的耐久度
 * \param pThis 包裹主人
 * \param value 装备耐久值
 */
void EquipPack::updateDurability(SceneUser *pThis , DWORD value)
{
	for (int i=0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL)
		{
			container[i]->data.dur = value>container[i]->data.maxdur?container[i]->data.maxdur:value;
           //soke 这里是装备耐久度
//////////////////////////////////////////////////////
			if(container[i]->data.dur == 0)
			{
				container[i]->data.dur = 1;
			}
//////////////////////////////////////////////////////
			freshDurability(pThis, container[i]);

			/*
			if(equip[i]!=NULL && equip[i]->data.dur < equip[i]->data.maxdur)
			{
				equip[i]->data.dur = equip[i]->data.maxdur;
			}
			*/
		}
	}
}

/**
 * \brief 将装备信息填到buf中
 * \param buf 返回空间
 * \return 装备数量
 */
DWORD EquipPack::fullAllEquiped(char *buf)
{
	using namespace Cmd;
	EquipedObject *data = (EquipedObject*)buf;
	int num = 0;
	for (int i =0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL)
		{
			bcopy(&container[i]->data , &data[num].object, sizeof(t_Object));
			data[i].byWhere = i;
			num++;
		}
	}
	return num;
}

/**
 * \brief 恢复耐久度
 * \param pThis 角色
 * \param ct 时间
 */
void EquipPack::restituteDurability(SceneUser *pThis , const zRTime &ct)
{

	for (int i=0;i<12;i++)
	{
		if(container[i]!=NULL)
		{
			if(container[i]!=NULL && container[i]->data.dur < container[i]->data.maxdur)
			{
				if(container[i]->data.dursecond > 0 && container[i]->data.durpoint > 0)
				{
					if(!(ct.sec() % container[i]->data.dursecond))
					{
						bool need = container[i]->data.dur == 0 ? true : false;
						container[i]->data.dur +=container[i]->data.durpoint;
						if(container[i]->data.dur > container[i]->data.maxdur)
						{
							container[i]->data.dur = container[i]->data.maxdur;
						}
						if(need)
						{
							freshDurability(pThis , container[i]);
							calcAll();
							freshUserData(pThis);
						}
					}
				}
			}
		}
	}

}

/**
 * \brief 重算装备对人物的影响并通知客户端
 * \param 
 * \return 
 */
void EquipPack::freshUserData(SceneUser *pThis)
{
	pThis->setupCharBase();
	Cmd::stMainUserDataUserCmd  userinfo;
	pThis->full_t_MainUserData(userinfo.data);
	pThis->sendCmdToMe(&userinfo,sizeof(userinfo));
}

/**
 * \brief 刷新耐久度，通知客户端耐久度变化
 * \param pThis 角色
 * \param o 物品
  */
void EquipPack::freshDurability(SceneUser *pThis , zObject *o)
{
	Cmd::stDurabilityUserCmd std;
	std.dwThisID = o->data.qwThisID;
	std.dwDur = o->data.dur;
	std.dwMaxDur = o->data.maxdur;
	pThis->sendCmdToMe(&std , sizeof(std));
}

/**
 * \brief 削减耐久度 
 * \param pThis 角色
 * \param which 消耗目标
 */
bool EquipPack::reduceDur(SceneUser *pThis , DWORD which)
{
	if(which > 15 || container[which] == NULL)
	{
		return false;
	}
	WORD olddur = (WORD)(container[which]->data.dur / 50) + 1;
	container[which]->data.dur --;
	if((olddur != (WORD)(container[which]->data.dur / 50) + 1) || (container[which]->data.dur == 0))
	{
		freshDurability(pThis , container[which]);
	}
	if(container[which]->data.dur == 0)
	{
		calcAll();

		//soke 根据策划要求，装备耐久为0时不消失，只是不可用啦
		//if (container[which]->base->kind <= 118 && container[which]->base->kind >= 101 && container[which]->data.bind)
		//{
		//	return false;
		//}
		//else
		//{
		//	//临时增加,便于QA测试
		//	//再次根据策划文档修改
		//	zObject::logger(container[which]->createid,container[which]->data.qwThisID,container[which]->data.strName,container[which]->data.dwNum,1,0,pThis->id,pThis->name,0,NULL,"耐久用完删除",NULL,0,0);
		//	if (pThis->packs.removeObject(container[which]))
		//	{
		//		container[which]=NULL; 
		//		pThis->setupCharBase();
		//		Cmd::stMainUserDataUserCmd  userinfo;
		//		pThis->full_t_MainUserData(userinfo.data);
		//		pThis->sendCmdToMe(&userinfo,sizeof(userinfo));
		//		pThis->sendMeToNine();
		//	}

		//	return false;
		//}
	}
	return true;
}
zObject *EquipPack::getObjectByEquipPos(DWORD no)
{
	if(no < 16)
	{
		return container[no];
	}
	else
	{
		return NULL;
	}
}
zObject *EquipPack::getObjectByEquipNo(EQUIPNO no)
{
	return container[no];
}

zObject *EquipPack::getObjectByType(int type)
{
	for (int i=0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			if(container[i]->base->kind == type)
			{
				return container[i];
			}
		
		}
	}
	return NULL;
}
/**
 * \brief 削减耐久值 
 * \param pThis 角色
 * \param which 消耗目标
 *
 * \return 实际消耗的耐久值
 */
int EquipPack::reduceDur(SceneUser *pThis , DWORD which , DWORD type , DWORD num , bool needFresh , bool needCal)
{
	int ret = 0;
	if(which > 15 || container[which] == NULL)
	{
		return 0;
	}
	if(container[which]->base->kind != type)
	{
		return 0;
	}
	WORD olddur = (WORD)(container[which]->data.dur / 50) + 1;
	ret = (int)(container[which]->data.dur - num);
	if(ret > 0)
	{
		container[which]->data.dur = ret;
		ret = num;
	}
	else
	{
		ret = container[which]->data.dur;
		container[which]->data.dur = 0;
	}
	if(needFresh)
	{
			freshDurability(pThis , container[which]);
	}
	else
	{
		if(olddur != (WORD)(container[which]->data.dur / 50) + 1)
		{
			freshDurability(pThis , container[which]);
		}
	}
	if(container[which]->data.dur == 0)
	{
		if(needCal)
		{
			calcAll();
		}

		//临时增加,便于QA测试
		//再次根据策划文档修改
		if( (container[which]->base->kind >= 101 && container[which]->base->kind <= 118) || (container[which]->base->kind >= 130 && container[which]->base->kind <= 138)|| (container[which]->base->kind >= 141 && container[which]->base->kind <= 147)|| container[which]->base->kind == 136 || container[which]->base->kind == 137|| container[which]->base->kind == 155 &&& container[which]->data.bind)
		{
			return false;
		}
		else
		{
			zObject::logger(container[which]->createid,container[which]->data.qwThisID,container[which]->data.strName,container[which]->data.dwNum,1,0,pThis->id,pThis->name,0,NULL,"耐久用完删除",NULL,0,0);
			if(pThis->packs.removeObject(container[which])) //notify and delete
			{
				container[which]=NULL; 
				pThis->setupCharBase();
				Cmd::stMainUserDataUserCmd  userinfo;
				pThis->full_t_MainUserData(userinfo.data);
				pThis->sendCmdToMe(&userinfo,sizeof(userinfo));
				pThis->sendMeToNine();
			}
		}

	}
	return ret;
}

/**
 * \brief 消耗攻击性装备耐久度
 * \param pThis 角色
 */
bool EquipPack::costAttackDur(SceneUser *pThis)
{
	bool bret = false;
	//int jewelry = 0;
	//int which[3] ={0};
	//
	for (int i=0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			switch(container[i]->base->kind)
			{
				case ItemType_Blade:		    //104代表武术刀类武器
				case ItemType_Sword :	        //105代表武术剑类武器
				case ItemType_Axe:	    	    //106代表武术斧类武器
				case ItemType_Hammer:	        //107代表武术斧类武器
				case ItemType_Staff:		    //108代表法术杖类武器
				case ItemType_Crossbow:	        //109代表箭术弓类武器
				case ItemType_Fan:	   	        //110代表美女扇类
				case ItemType_Stick:	        //111代表召唤棍类武器
				case ItemType_Flower:			//120代表鲜花,采集手套...
				case ItemType_Dagger:	        //136代表利刃
			    case ItemType_NewSword:	        //137代表卫士剑
				case ItemType_LongGun:          //155代表长枪
					{
						if(zMisc::selectByPercent(0))
						{
							if (container[i] && container[i]->base->recast && container[i]->data.dur<=50)
								pThis->petAutoRepair(container[i]);

							if(!reduceDur(pThis , i))
							{
								bret = true;
							}
						}
					}
					break;
					/*
				case ItemType_Necklace:
				case ItemType_Fing:
					{
						jewelry ++;
						if(jewelry <= 3)
						{
							which[jewelry - 1] = i;
						}
					}
					break;
					// */
				default:
					break;
			}
		}
	}

	/*
	for(int ii = 0 ; ii < jewelry ; ii ++)
	{
		if(zMisc::selectByPercent(60))
		{
			if(!reduceDur(pThis , which[ii]))
			{
				bret = true;
			}
		}
	}
	// */
	/*
	switch(jewelry)
	{
		case 1:
			{
				if(!reduceDur(pThis , which[jewelry -1]))
				{
					bret = true;
				}
			}
			break;
		case 2:
			{
				jewelry -= zMisc::randBetween(0 , 1);
				if(!reduceDur(pThis , which[jewelry - 1]))
				{
					bret = true;
				}
			}
			break;
		case 3:
			{
				jewelry -= zMisc::randBetween(0 , 2);
				if(!reduceDur(pThis , which[jewelry - 1]))
				{
					bret = true;
				}
			}
			break;
		default:
			break;
	}
	// */
	if(bret)
	{
		freshUserData(pThis);
	}
	return bret;
}

/**
 * \brief 宠物攻击消耗攻击性装备耐久度
 * \param pThis 角色
 */
bool EquipPack::costAttackDurByPet(SceneUser *pThis)
{
	bool bret = false;
	//int jewelry = 0;
	//int which[3] ={0};
	//
	for (int i=0;i<16;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			switch(container[i]->base->kind)
			{
				case ItemType_Stick:	        //111代表召唤棍类武器
					{
						if(zMisc::selectByPercent(0))
						{
							if (container[i])
							{
								if (container[i]->base->level <= 70) continue;
								if (container[i]->data.dur<=50 && container[i]->base->recast)
									pThis->petAutoRepair(container[i]);
							}

							if(!reduceDur(pThis , i))
							{
								bret = true;
							}
						}
					}
					break;
				default:
					break;
			}
		}
	}

	if(bret)
	{
		freshUserData(pThis);
	}
	return bret;
}

/**
 * \brief 消耗防御性装备耐久度
 * \param pThis 角色
 */
bool EquipPack::costDefenceDur(SceneUser *pThis)
{
	bool bret = false;
	int clothing = 0;
	int which[5] = {0};
	for (int i=0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			switch(container[i]->base->kind)
			{
				case ItemType_ClothBody:		//101代表布质加生命类服装
				case ItemType_FellBody:		    //102代表皮甲加魔防类服装
				case ItemType_MetalBody:		//103代表金属铠甲加物防类服装
				case ItemType_Helm:		        //113代表角色头盔类
				case ItemType_Caestus:	        //114代表角色腰带类
				case ItemType_Cuff:		        //115代表角色护腕类
				case ItemType_Shoes:		    //116代表角色鞋子类
				case ItemType_HorseRope:        //131代表坐骑马绳类
			    case ItemType_HorseSaddle:      //132代表坐骑马鞍类
					{
						/*
						if(clothing < 5)
						{
							which[clothing] = i;
						}
						// */
						which[clothing] = i;
						clothing ++;
					}
					break;
				case ItemType_Shield:
				case ItemType_Necklace:
				case ItemType_Fing:
			    case ItemType_HorseShoe:          //130代表坐骑马脚类
			    case ItemType_HorseSafe:          //133代表坐骑马甲类
			    case ItemType_HorseIron:          //134代表坐骑马镫类
				case ItemType_breastplate:	      //138代表护心镜
	        	case ItemType_StaffFlag:		  //141代表战旗(法师)
	          //case ItemType_CrossFlag:	      //142代表战旗(弓手) 取消耐久
	        	case ItemType_Flag:	              //143代表战旗(仙术)
	        	case ItemType_StickFlag:	      //144代表战旗(召唤)
	        	case ItemType_BladeFlag:	      //145代表战旗(战士)
	        	case ItemType_DaggerFlag:	      //146代表战旗(刺客)
	        	case ItemType_SwordFlag:          //147代表战旗(卫士)
	        	case ItemType_Handgun:            //156代表短枪
					{
						if(zMisc::selectByPercent(0))
						{
							if (container[i] && container[i]->base->recast && container[i]->data.dur<=50)
								pThis->petAutoRepair(container[i]);
							if(!reduceDur(pThis , i))
							{
								bret = true;
							}
						}
					}
					break;
				default:
					break;
			}
		}
	}
	for(int ii=0 ; ii < clothing ; ii ++)
	{
		if(zMisc::selectByPercent(3))
		{
			if (container[which[ii]] && container[which[ii]]->base->recast && container[which[ii]]->data.dur<=50)
				pThis->petAutoRepair(container[which[ii]]);

			if(!reduceDur(pThis , which[ii]))
			{
				bret = true;
			}
		}
	}
	/*
	switch(clothing)
	{
		case 5:
			{
				int except_1 = zMisc::randBetween(0 , 4);
				int except_2 = zMisc::randBetween(0 , 4);
				while(except_2 == except_1)
				{
					except_2 = zMisc::randBetween(0 , 4);
				}
				for(int i = 0 ; i < clothing ; i ++)
				{
					if(i != except_1 && i != except_2)
					{
						if(!reduceDur(pThis , which[i]))
						{
							bret = true;
						}
					}
				}
			}
			break;
		case 4:
			{
				int except = zMisc::randBetween(0 , 3);
				for(int i = 0 ; i < clothing ; i ++)
				{
					if(i != except)
					{
						if(!reduceDur(pThis , which[i]))
						{
							bret = true;
						}
					}
				}
			}
			break;
		case 3:
		case 2:
		case 1:
			{
				for(int i = 0 ; i < clothing ; i ++)
				{
					if(!reduceDur(pThis , which[i]))
					{
						bret = true;
					}
				}
			}
			break;
		default:
			break;
	}
	// */
	if(bret)
	{
		freshUserData(pThis);
	}
	return bret;
}

/**
 * \brief 宠物攻击消耗防御性装备耐久度
 * \param pThis 角色
 */
bool EquipPack::costDefenceDurByPet(SceneUser *pThis)
{
	bool bret = false;

	for (int i=0;i<16;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			switch(container[i]->base->kind)
			{
				case ItemType_Necklace:
				case ItemType_Fing:
					{
						if(zMisc::selectByPercent(0))
						{
							if (container[i])
							{
								if (container[i]->base->level <=70) continue;
								if (container[i]->data.dur<=50 && container[i]->base->recast)
									pThis->petAutoRepair(container[i]);
							}
							if(!reduceDur(pThis , i))
							{
								bret = true;
							}
						}
					}
					break;
				default:
					break;
			}
		}
	}

	if(bret)
	{
		freshUserData(pThis);
	}
	return bret;
}

/**
 * \brief 获取虚拟装备
 * \return 虚拟装备对象
 */
const Equips& EquipPack::getEquips() const
{
	return equips;
}

#define CALCUTE(prop) equips.prop += container[i]->data.prop;
 
/**
 * \brief 重算虚拟装备数值
 */
void EquipPack::calcAll() 
{
	bool calcmdamage;
	equips.reset();
	typedef std::map<std::string,int> SuitNmaeMap;
	typedef SuitNmaeMap::iterator SuitNameMap_iter;
	SuitNmaeMap suitmap[3];
	std::pair<int,int> suitnum;
	suitnum.first=-1;
	suitnum.second=0;

	calcmdamage = true;
	if (container[position(0,Cmd::EQUIPCELLTYPE_HANDR)])
	{
		if (container[position(0,Cmd::EQUIPCELLTYPE_HANDR)]->base->kind == ItemType_Stick)
		{
			//soke 解决棍不增加魔法攻击BUG
			//calcmdamage = false;
			  calcmdamage = true;
		}
	}
	//soke 这个是时装装备的位置数，真实有效的
	for (int i=0;i<Cmd::EQUIPCELLTYPE_MAX;i++)
	{
		if(container[i]!=NULL && container[i]->data.dur != 0)
		{
			//soke 时装直接增加百分比
			/*
			if (container[i]->base->kind == ItemType_FashionBody || container[i]->base->kind == ItemType_HighFashionBody)
			{
			    equips.pdam+=container[i]->data.maxpdamage; 	// 增加物理攻击力x%
				equips.mdam+=container[i]->data.maxmdamage;  	// 增加魔法攻击力x%
				equips.maxhprate +=container[i]->data.maxhp; 	// 增加生命值x%
				equips.maxmprate +=container[i]->data.maxmp; 	// 增加法术值x%

			}
			*/
			//soke 新增加的新型时装支持OB控制固定加血
			if ( container[i]->base->kind == ItemType_XHighFashionBody || container[i]->base->kind == ItemType_SHighFashionBody || container[i]->base->kind == ItemType_FashionBody || container[i]->base->kind == ItemType_HighFashionBody)
			{
			    equips.pdam+=container[i]->data.maxpdamage; 	// 增加物理攻击力x%
				equips.mdam+=container[i]->data.maxmdamage;  	// 增加魔法攻击力x%
				equips.maxhprate +=container[i]->data.maxhp; 	// 增加生命值x%
				equips.maxmprate +=container[i]->data.maxmp; 	// 增加法术值x%
			 //	equips.maxhp += DWORD( 10000 ); 
				equips.maxhp += DWORD(container[i]->data.maxsp); 	// 增加额外体力值OB的防御改为血量				

			}

			//myy 修改马牌由原来的百分比改为固定值但是血量是百分比
			if ( container[i]->base->kind == ItemType_HorseFashion )
			{
					equips.maxhprate += DWORD(container[i]->data.maxhp); // 增加百分比生命值上限
					equips.maxmp += DWORD(container[i]->data.maxmp);     // 增加固定法术值上限
			        equips.pdam+= DWORD(container[i]->data.maxpdamage); 	 // 增加物理攻击力x%
				    equips.mdam+= DWORD(container[i]->data.maxmdamage);   // 增加魔法攻击力x%
					equips.pdef+= DWORD(container[i]->data.pdefence);   //增加物防x%
					equips.mdef+= DWORD(container[i]->data.mdefence);   //增加魔防x%
					

			}

			//sky 神器单件加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 141 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
				if (container[i]->data.retain3 ==7)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp);
					equips.maxmp += DWORD(container[i]->data.maxmp);
					equips.pdamage += DWORD(container[i]->data.pdamage);
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage);
					equips.mdamage += DWORD(container[i]->data.mdamage);
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage);
				}
				else if (container[i]->data.retain3 ==6)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(85/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(85/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(85/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(85/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(85/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(85/100.0f));
				}
				else if (container[i]->data.retain3 ==5)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(85/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(85/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(85/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(85/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(85/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(85/100.0f));
				}
				else if (container[i]->data.retain3 ==4)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(55/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(55/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(55/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(55/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(55/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(55/100.0f));
				}
				else if (container[i]->data.retain3 ==3)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(40/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(40/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(40/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(40/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(40/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(40/100.0f));
				}
				else if (container[i]->data.retain3 ==2)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(25/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(25/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(25/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(25/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(25/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(25/100.0f));
				}
				else if (container[i]->data.retain3 ==1)
				{
					equips.maxhp += DWORD(container[i]->data.maxhp *(10/100.0f));
					equips.maxmp += DWORD(container[i]->data.maxmp *(10/100.0f));
					equips.pdamage += DWORD(container[i]->data.pdamage *(10/100.0f));
					equips.maxpdamage += DWORD(container[i]->data.maxpdamage *(10/100.0f));
					equips.mdamage += DWORD(container[i]->data.mdamage *(10/100.0f));
					equips.maxmdamage += DWORD(container[i]->data.maxmdamage *(10/100.0f));
				}
			}
			
			//sky 龙槽龙星加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 141 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
				equips.pdamage += (DWORD)(container[i]->data.pdamage *(container[i]->data.p4damage + container[i]->data.p5damage)/100.0f);           //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.maxpdamage *(container[i]->data.m4axpdamage + container[i]->data.m5axpdamage)/100.0f);   //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.mdamage *(container[i]->data.m4damage + container[i]->data.m5damage)/100.0f);           //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.maxmdamage *(container[i]->data.m4axmdamage + container[i]->data.m5axmdamage)/100.0f);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.pdefence *(container[i]->data.p4defence + container[i]->data.p5defence)/100.0f);      //物防
				equips.mdefence += (DWORD)(container[i]->data.mdefence *(container[i]->data.m4defence + container[i]->data.m5defence)/100.0f);     //魔防
				equips.maxhp += (DWORD)(container[i]->data.maxhp *(container[i]->data.m4axhp + container[i]->data.m5axhp)/100.0f);                 //最大生命值

			}

			//sky 神器加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 141 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
				equips.pdamage += (DWORD)(container[i]->data.pdamage *(container[i]->data.p9damage)/100.0f);           //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.maxpdamage *(container[i]->data.m9axpdamage )/100.0f);   //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.mdamage *((container[i]->data.m9damage)/100.0f));           //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.maxmdamage *(container[i]->data.m9axmdamage)/100.0f);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.pdefence *(container[i]->data.p9defence)/100.0f);      //物防
				equips.mdefence += (DWORD)(container[i]->data.mdefence *(container[i]->data.m9defence)/100.0f);     //魔防
				equips.maxhp += (DWORD)(container[i]->data.maxhp *(container[i]->data.m9axhp)/100.0f);                 //最大生命值

			}

			//sky 龙凤吟镶嵌效果加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 108) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 141 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
				equips.pdamage += (DWORD)(container[i]->data.p24damage);           //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m24axpdamage);   //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m24damage);           //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m24axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p24defence);      //物防
				equips.mdefence += (DWORD)(container[i]->data.m24defence);     //魔防
				equips.maxhp += (DWORD)(container[i]->data.m24axhp);                 //最大生命值

			}

			//sky 精炼加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 140 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
                equips.pdamage += (DWORD)(container[i]->data.p10damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m10axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m10damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m10axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p10defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m10defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m10axhp);           //最大生命值
			}

			
			//sky 神龙镶嵌加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 140 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
                equips.pdamage += (DWORD)(container[i]->data.p14damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m14axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m14damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m14axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p14defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m14defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m14axhp);           //最大生命值
			}

			//sky 逆天进阶加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 140 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
                equips.pdamage += (DWORD)(container[i]->data.p15damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m15axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m15damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m15axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p15defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m15defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m15axhp);           //最大生命值
			}

			//sky 神魂宝石斗战神佛进阶加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 140 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
                equips.pdamage += (DWORD)(container[i]->data.p16damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m16axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m16damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m16axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p16defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m16defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m16axhp);           //最大生命值
			}

			//sky 暗影宝石加成
			if ((container[i]->base->kind >= 101 && container[i]->base->kind <= 118) || (container[i]->base->kind >= 130 && container[i]->base->kind <= 138)
				|| (container[i]->base->kind >= 140 && container[i]->base->kind <= 147) || container[i]->base->kind == 155 || container[i]->base->kind == 156)
			{
                equips.pdamage += (DWORD)(container[i]->data.p17damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m17axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m17damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m17axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p17defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m17defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m17axhp);           //最大生命值
			}

			//sky 护身符璀璨加成
			if (container[i]->base->kind == 80)
			{
                equips.pdamage += (DWORD)(container[i]->data.p11damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m11axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m11damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m11axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p11defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m11defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m11axhp);           //最大生命值
			}
			//sky 护身符神兽宝石加成
			if (container[i]->base->kind == 80)
			{
                equips.pdamage += (DWORD)(container[i]->data.p12damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m12axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m12damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m12axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p12defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m12defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m12axhp);           //最大生命值
			}
			//sky 百兽图鉴计算加成
			if (container[i]->base->kind >= 221 && container[i]->base->kind <= 230)
			{
				equips.maxhp += (DWORD)(container[i]->data.m13axhp);           //最大生命值
                equips.pdamage += (DWORD)(container[i]->data.p13damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m13axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m13damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m13axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p13defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m13defence);    //魔防

			}	
		    //sky 尚方宝剑进阶加成
			if (container[i]->base->kind == 88)
			{
                equips.pdamage += (DWORD)(container[i]->data.p18damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m18axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m18damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m18axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p18defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m18defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m18axhp);           //最大生命值
			}
			//sky 紫金龙袍进阶加成
			if (container[i]->base->kind == 89)
			{
                equips.pdamage += (DWORD)(container[i]->data.p19damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m19axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m19damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m19axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p19defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m19defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m19axhp);           //最大生命值
			}				
			
			//sky 法宝和符石进阶加成
			if (container[i]->base->kind >= ItemType_Fabao && container[i]->base->kind <= ItemType_Fabaofj12)
			{
                equips.pdamage += (DWORD)(container[i]->data.p20damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m20axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m20damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m20axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p20defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m20defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m20axhp);           //最大生命值
			}		

			//sky 十二星宿进阶加成
			if (container[i]->base->kind >= ItemType_SEXX1 && container[i]->base->kind <= ItemType_SEXX12)
			{
                equips.pdamage += (DWORD)(container[i]->data.p21damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m21axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m21damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m21axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p21defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m21defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m21axhp);           //最大生命值
			}		

			//sky 元神进阶加成
			if (container[i]->base->kind >= ItemType_YUANSHEN && container[i]->base->kind <= ItemType_YUANSHENFJ6)
			{
                equips.pdamage += (DWORD)(container[i]->data.p22damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m22axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m22damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m22axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p22defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m22defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m22axhp);           //最大生命值
			}		
			//sky 元神进阶加成
			if (container[i]->base->kind >= ItemType_YUANSHEN && container[i]->base->kind <= ItemType_YUANSHENFJ6)
			{
                equips.pdamage += (DWORD)(container[i]->data.p23damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m23axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m23damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m23axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p23defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m23defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m23axhp);           //最大生命值
			}		

			
			//sky 护身符神斧加成
			if (container[i]->base->kind == 80)
			{
                equips.pdamage += (DWORD)(container[i]->data.p32damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m32axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m32damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m32axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p32defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m32defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m32axhp);           //最大生命值
			}

			//sky 护身符黑曜加成
			if (container[i]->base->kind == 80)
			{
                equips.pdamage += (DWORD)(container[i]->data.p33damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m33axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m33damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m33axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p33defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m33defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m33axhp);           //最大生命值
			}

			//sky 护身符精金加成
			if (container[i]->base->kind == 80)
			{
                equips.pdamage += (DWORD)(container[i]->data.p34damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m34axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m34damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m34axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p34defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m34defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m34axhp);           //最大生命值
			}
			
			//sky 金箍咒强化加成
			if (container[i]->base->kind == 90)
			{
                equips.pdamage += (DWORD)(container[i]->data.p49damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m49axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m49damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m49axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p49defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m49defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m49axhp);           //最大生命值
			}
			//sky 金箍咒字纹加成
			if (container[i]->base->kind == 90)
			{
                equips.pdamage += (DWORD)(container[i]->data.p50damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m50axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m50damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m50axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p50defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m50defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m50axhp);           //最大生命值
			}
			//sky 金箍咒淬炼百分比加成
			if (container[i]->base->kind == 90)
			{											
				equips.pdamage += (DWORD)((container[i]->data.p7damage + container[i]->data.p49damage + container[i]->data.p50damage) *(container[i]->data.p51damage)/100.0f);                     //最小物理攻击力
				equips.maxpdamage += (DWORD)((container[i]->data.m7axpdamage + container[i]->data.m49axpdamage + container[i]->data.m50axpdamage) *(container[i]->data.m51axpdamage)/100.0f);   //最大物理攻击力
				equips.mdamage += (DWORD)((container[i]->data.m7damage + container[i]->data.m49damage + container[i]->data.m50damage) *(container[i]->data.m51damage)/100.0f);                     //最小魔法攻击力
				equips.maxmdamage += (DWORD)((container[i]->data.m7axmdamage + container[i]->data.m49axmdamage + container[i]->data.m50axmdamage) *(container[i]->data.m51axmdamage)/100.0f);   //最大魔法攻击力
				equips.pdefence += (DWORD)((container[i]->data.p7defence + container[i]->data.p49defence + container[i]->data.p50defence) *(container[i]->data.p51defence)/100.0f);               //物防
				equips.mdefence += (DWORD)((container[i]->data.m7defence + container[i]->data.m49defence + container[i]->data.m50defence) *(container[i]->data.m51defence)/100.0f);               //魔防
				equips.maxhp += (DWORD)((container[i]->data.m7axhp + container[i]->data.m49axhp + container[i]->data.m50axhp) *(container[i]->data.m51axhp)/100.0f);                                 //最大生命值              
			}				
			//sky 寒冰神炼加成
			if (container[i]->base->kind == 91)
			{
                equips.pdamage += (DWORD)(container[i]->data.p52damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m52axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m52damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m52axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p52defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m52defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m52axhp);           //最大生命值
			}
			//sky 寒冰字纹加成
			if (container[i]->base->kind == 91)
			{
                equips.pdamage += (DWORD)(container[i]->data.p53damage);   //最小物理攻击力
				equips.maxpdamage += (DWORD)(container[i]->data.m53axpdamage);  //最大物理攻击力
				equips.mdamage += (DWORD)(container[i]->data.m53damage);         //最小魔法攻击力
				equips.maxmdamage += (DWORD)(container[i]->data.m53axmdamage);  //最大魔法攻击力
				equips.pdefence += (DWORD)(container[i]->data.p53defence);    //物防
				equips.mdefence += (DWORD)(container[i]->data.m53defence);    //魔防
				equips.maxhp += (DWORD)(container[i]->data.m53axhp);           //最大生命值
			}
			//sky 寒冰纹章百分比加成
			if (container[i]->base->kind == 91)
			{											
				equips.pdamage += (DWORD)((container[i]->data.p8damage + container[i]->data.p52damage + container[i]->data.p53damage) *(container[i]->data.p54damage)/100.0f);                     //最小物理攻击力
				equips.maxpdamage += (DWORD)((container[i]->data.m8axpdamage + container[i]->data.m52axpdamage + container[i]->data.m53axpdamage) *(container[i]->data.m54axpdamage)/100.0f);   //最大物理攻击力
				equips.mdamage += (DWORD)((container[i]->data.m8damage + container[i]->data.m52damage + container[i]->data.m53damage) *(container[i]->data.m54damage)/100.0f);                     //最小魔法攻击力
				equips.maxmdamage += (DWORD)((container[i]->data.m8axmdamage + container[i]->data.m52axmdamage + container[i]->data.m53axmdamage) *(container[i]->data.m54axmdamage)/100.0f);   //最大魔法攻击力
				equips.pdefence += (DWORD)((container[i]->data.p8defence + container[i]->data.p52defence + container[i]->data.p53defence) *(container[i]->data.p54defence)/100.0f);               //物防
				equips.mdefence += (DWORD)((container[i]->data.m8defence + container[i]->data.m52defence + container[i]->data.m53defence) *(container[i]->data.m54defence)/100.0f);               //魔防
				equips.maxhp += (DWORD)((container[i]->data.m8axhp + container[i]->data.m52axhp + container[i]->data.m53axhp) *(container[i]->data.m54axhp)/100.0f);                                 //最大生命值              
			}	
			if(container[i]->data.bind && container[i]->data.maker[0] && container[i]->base->kind != ItemType_Shield)
			{
				int color = -1;
				if(container[i]->data.kind & 1) //soke 这里跟下面的五行套装对应
				{
					color = 0;
				}
				if (container[i]->data.kind & 2)
				{
					color = 1;
				}
				if (container[i]->data.kind & 4)
				{
					color = 2;
				}
				if(color != -1)
				{
					SuitNameMap_iter iter = suitmap[color].find(container[i]->data.maker);
					if(iter != suitmap[color].end())
					{
						iter->second ++;
						if( iter->second >=6 )
						{
							suitnum.first = color;
							suitnum.second = iter->second;
						}
					}
					else
					{
						suitmap[color].insert(std::make_pair(container[i]->data.maker,1));
					}
				}
			}
			CALCUTE(maxhp)		CALCUTE(maxhp)					// 最大生命值
			CALCUTE(m1axhp)					// 最大生命值
			CALCUTE(m2axhp)					// 最大生命值
			CALCUTE(m3axhp)					// 斗魂最大生命值
			CALCUTE(m4axhp)					// 龙槽最大生命值
			CALCUTE(m5axhp)					// 龙星最大生命值
			CALCUTE(m6axhp)					// 补天最大生命值
			CALCUTE(m7axhp)					// 勋章，神器，神兽最大生命值
			CALCUTE(m8axhp)					// 寒冰，翅膀最大生命值
			CALCUTE(m9axhp)					// 神器最大生命值
			CALCUTE(m10axhp)				// 精炼最大生命值
			CALCUTE(m11axhp)				// 璀璨镶嵌最大生命值
			CALCUTE(m12axhp)				// 神兽宝石最大生命值
			CALCUTE(m13axhp)				// 百兽图鉴 最大生命值
			CALCUTE(m14axhp)				// 神龙镶嵌 最大生命值
			CALCUTE(m15axhp)				// 逆天进阶 最大生命值
			CALCUTE(m16axhp)				// 神魂宝石 最大生命值
			CALCUTE(m17axhp)				// 暗影宝石 最大生命值
			CALCUTE(m18axhp)				// 预留功能 最大生命值
			CALCUTE(m19axhp)				// 预留功能 最大生命值
			CALCUTE(m20axhp)				// 预留功能 最大生命值
			CALCUTE(m21axhp)				// 预留功能 最大生命值
			CALCUTE(m22axhp)				// 预留功能 最大生命值
			CALCUTE(m23axhp)				// 预留功能 最大生命值
			CALCUTE(m24axhp)				// 预留功能 最大生命值
			CALCUTE(m25axhp)				// 预留功能 最大生命值
			CALCUTE(m26axhp)				// 预留功能 最大生命值
			CALCUTE(m27axhp)				// 预留功能 最大生命值
			CALCUTE(m28axhp)				// 预留功能 最大生命值
			CALCUTE(m29axhp)				// 预留功能 最大生命值
			CALCUTE(m30axhp)				// 预留功能 最大生命值
			CALCUTE(m31axhp)				// 预留功能 最大生命值
			CALCUTE(m32axhp)				// 预留功能 最大生命值
			CALCUTE(m33axhp)				// 预留功能 最大生命值
			CALCUTE(m34axhp)				// 预留功能 最大生命值
			CALCUTE(m35axhp)				// 预留功能 最大生命值
			CALCUTE(m36axhp)				// 预留功能 最大生命值
			CALCUTE(m37axhp)				// 预留功能 最大生命值
			CALCUTE(m38axhp)				// 预留功能 最大生命值
			CALCUTE(m39axhp)				// 预留功能 最大生命值
			CALCUTE(m40axhp)				// 预留功能 最大生命值
			CALCUTE(m41axhp)				// 预留功能 最大生命值
			CALCUTE(m42axhp)				// 预留功能 最大生命值
			CALCUTE(m43axhp)				// 预留功能 最大生命值
			CALCUTE(m44axhp)				// 预留功能 最大生命值
			CALCUTE(m45axhp)				// 预留功能 最大生命值
			CALCUTE(m46axhp)				// 预留功能 最大生命值
			CALCUTE(m47axhp)				// 预留功能 最大生命值
			CALCUTE(m48axhp)				// 预留功能 最大生命值
			CALCUTE(m49axhp)				// 预留功能 最大生命值
			CALCUTE(m50axhp)				// 预留功能 最大生命值
			CALCUTE(m51axhp)				// 预留功能 最大生命值
			CALCUTE(m52axhp)				// 预留功能 最大生命值
			CALCUTE(m53axhp)				// 预留功能 最大生命值
			CALCUTE(m54axhp)				// 预留功能 最大生命值
			CALCUTE(m55axhp)				// 预留功能 最大生命值
			CALCUTE(m56axhp)				// 预留功能 最大生命值
			CALCUTE(m57axhp)				// 预留功能 最大生命值
			CALCUTE(m58axhp)				// 预留功能 最大生命值
			CALCUTE(m59axhp)				// 预留功能 最大生命值
			CALCUTE(m60axhp)				// 预留功能 最大生命值
			CALCUTE(m61axhp)				// 预留功能 最大生命值
			CALCUTE(m62axhp)				// 预留功能 最大生命值
			CALCUTE(m63axhp)				// 预留功能 最大生命值
			CALCUTE(m64axhp)				// 预留功能 最大生命值
			CALCUTE(m65axhp)				// 预留功能 最大生命值
			CALCUTE(m66axhp)				// 预留功能 最大生命值
			CALCUTE(m67axhp)				// 预留功能 最大生命值
			CALCUTE(m68axhp)				// 预留功能 最大生命值
			CALCUTE(m69axhp)				// 预留功能 最大生命值
			CALCUTE(m70axhp)				// 预留功能 最大生命值
			CALCUTE(m71axhp)				// 预留功能 最大生命值
			CALCUTE(m72axhp)				// 预留功能 最大生命值
			CALCUTE(m73axhp)				// 预留功能 最大生命值
			CALCUTE(m74axhp)				// 预留功能 最大生命值
			CALCUTE(m75axhp)				// 预留功能 最大生命值
			CALCUTE(m76axhp)				// 预留功能 最大生命值
			CALCUTE(m77axhp)				// 预留功能 最大生命值
			CALCUTE(m78axhp)				// 预留功能 最大生命值
			CALCUTE(m79axhp)				// 预留功能 最大生命值
			CALCUTE(m80axhp)				// 预留功能 最大生命值
			CALCUTE(m81axhp)				// 预留功能 最大生命值
			CALCUTE(m82axhp)				// 预留功能 最大生命值
			CALCUTE(m83axhp)				// 预留功能 最大生命值
			CALCUTE(m84axhp)				// 预留功能 最大生命值
			CALCUTE(m85axhp)				// 预留功能 最大生命值
			CALCUTE(m86axhp)				// 预留功能 最大生命值
			CALCUTE(m87axhp)				// 预留功能 最大生命值
			CALCUTE(m88axhp)				// 预留功能 最大生命值
			CALCUTE(m89axhp)				// 预留功能 最大生命值
			CALCUTE(m90axhp)				// 预留功能 最大生命值
			CALCUTE(m91axhp)				// 预留功能 最大生命值
			CALCUTE(m92axhp)				// 预留功能 最大生命值
			CALCUTE(m93axhp)				// 预留功能 最大生命值
			CALCUTE(m94axhp)				// 预留功能 最大生命值
			CALCUTE(m95axhp)				// 预留功能 最大生命值
			CALCUTE(m96axhp)				// 预留功能 最大生命值
			CALCUTE(m97axhp)				// 预留功能 最大生命值
			CALCUTE(m98axhp)				// 预留功能 最大生命值
			CALCUTE(m99axhp)				// 预留功能 最大生命值
			CALCUTE(m100axhp)				// 预留功能 最大生命值
			CALCUTE(m101axhp)				// 预留功能 最大生命值
			CALCUTE(m102axhp)				// 预留功能 最大生命值
			CALCUTE(m103axhp)				// 预留功能 最大生命值
			CALCUTE(m104axhp)				// 预留功能 最大生命值
			CALCUTE(m105axhp)				// 预留功能 最大生命值
			CALCUTE(m106axhp)				// 预留功能 最大生命值
			CALCUTE(m107axhp)				// 预留功能 最大生命值
			CALCUTE(m108axhp)				// 预留功能 最大生命值
			CALCUTE(m109axhp)				// 预留功能 最大生命值
			CALCUTE(m110axhp)				// 预留功能 最大生命值
			CALCUTE(m111axhp)				// 预留功能 最大生命值
			CALCUTE(m112axhp)				// 预留功能 最大生命值
			CALCUTE(m113axhp)				// 预留功能 最大生命值
			CALCUTE(m114axhp)				// 预留功能 最大生命值
			CALCUTE(m115axhp)				// 预留功能 最大生命值
			CALCUTE(m116axhp)				// 预留功能 最大生命值
			CALCUTE(m117axhp)				// 预留功能 最大生命值
			CALCUTE(m118axhp)				// 预留功能 最大生命值
			CALCUTE(m119axhp)				// 预留功能 最大生命值
			CALCUTE(m120axhp)				// 预留功能 最大生命值
			CALCUTE(m121axhp)				// 预留功能 最大生命值
			CALCUTE(m122axhp)				// 预留功能 最大生命值
			CALCUTE(m123axhp)				// 预留功能 最大生命值
			CALCUTE(m124axhp)				// 预留功能 最大生命值
			CALCUTE(m125axhp)				// 预留功能 最大生命值
			CALCUTE(m126axhp)				// 预留功能 最大生命值
			CALCUTE(m127axhp)				// 预留功能 最大生命值
			CALCUTE(m128axhp)				// 预留功能 最大生命值
			CALCUTE(m129axhp)				// 预留功能 最大生命值
			
			

			CALCUTE(maxmp)					// 最大法术值
			CALCUTE(maxsp)					// 最大体力值

            //soke 弓装备不需要装备箭支才能显示攻击
			//if (container[i]->base->kind != ItemType_Crossbow || (container[i]->base->kind == ItemType_Crossbow && equip(HANDR)/*arrow must*/ )) 
			//{
				CALCUTE(pdamage)				// 最小攻击力
				CALCUTE(maxpdamage)				// 最大攻击力
				CALCUTE(p1damage)				// 最小攻击力
				CALCUTE(m1axpdamage)			// 最大攻击力
				CALCUTE(p2damage)				// 最小攻击力
				CALCUTE(m2axpdamage)			// 最大攻击力
				CALCUTE(p3damage)				// 斗魂最小攻击力
				CALCUTE(m3axpdamage)			// 斗魂最大攻击力
				CALCUTE(p4damage)				// 龙槽最小攻击力
				CALCUTE(m4axpdamage)			// 龙槽最大攻击力
				CALCUTE(p5damage)				// 龙星最小攻击力
				CALCUTE(m5axpdamage)			// 龙星最大攻击力
				CALCUTE(p6damage)				// 补天最小攻击力
				CALCUTE(m6axpdamage)			// 补天最大攻击力
				CALCUTE(p7damage)				// 勋章，神兵，神兽最小攻击力
				CALCUTE(m7axpdamage)			// 勋章，神兵，神兽最大攻击力
				CALCUTE(p8damage)				// 寒冰，翅膀最小攻击力
				CALCUTE(m8axpdamage)			// 寒冰，翅膀最大攻击力
				CALCUTE(p9damage)				// 神器最小攻击力
				CALCUTE(m9axpdamage)			// 神器最大攻击力
				CALCUTE(p10damage)				// 马牌精炼 最小攻击力
				CALCUTE(m10axpdamage)			// 马牌精炼 最大攻击力
				CALCUTE(p11damage)				// 璀璨镶嵌 最小攻击力
				CALCUTE(m11axpdamage)			// 璀璨镶嵌 最大攻击力
				CALCUTE(p12damage)				// 神兽宝石 最小攻击力
				CALCUTE(m12axpdamage)			// 神兽宝石 最大攻击力

				CALCUTE(p13damage)				// 百兽图鉴 最小攻击力
				CALCUTE(m13axpdamage)			// 百兽图鉴 最大攻击力
				CALCUTE(p14damage)				// 神龙镶嵌 最小攻击力
				CALCUTE(m14axpdamage)			// 神龙镶嵌 最大攻击力
				CALCUTE(p15damage)				// 逆天进阶 最小攻击力
				CALCUTE(m15axpdamage)			// 逆天进阶 最大攻击力
				CALCUTE(p16damage)				// 神魂宝石 最小攻击力
				CALCUTE(m16axpdamage)			// 神魂宝石 最大攻击力
				CALCUTE(p17damage)				// 暗影宝石 最小攻击力
				CALCUTE(m17axpdamage)			// 暗影宝石 最大攻击力
				CALCUTE(p18damage)				// 尚方宝剑 最小攻击力
				CALCUTE(m18axpdamage)			// 尚方宝剑 最大攻击力
				CALCUTE(p19damage)				// 紫金龙袍 最小攻击力
				CALCUTE(m19axpdamage)			// 紫金龙袍 最大攻击力
				CALCUTE(p20damage)				// 预留功能 最小攻击力
				CALCUTE(m20axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p21damage)				// 预留功能 最小攻击力
				CALCUTE(m21axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p22damage)				// 预留功能 最小攻击力
				CALCUTE(m22axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p23damage)				// 预留功能 最小攻击力
				CALCUTE(m23axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p24damage)				// 预留功能 最小攻击力
				CALCUTE(m24axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p25damage)				// 预留功能 最小攻击力
				CALCUTE(m25axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p26damage)				// 预留功能 最小攻击力
				CALCUTE(m26axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p27damage)				// 预留功能 最小攻击力
				CALCUTE(m27axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p28damage)				// 预留功能 最小攻击力
				CALCUTE(m28axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p29damage)				// 预留功能 最小攻击力
				CALCUTE(m29axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p30damage)				// 预留功能 最小攻击力
				CALCUTE(m30axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p31damage)				// 预留功能 最小攻击力
				CALCUTE(m31axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p32damage)				// 预留功能 最小攻击力
				CALCUTE(m32axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p33damage)				// 预留功能 最小攻击力
				CALCUTE(m33axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p34damage)				// 预留功能 最小攻击力
				CALCUTE(m34axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p35damage)				// 预留功能 最小攻击力
				CALCUTE(m35axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p36damage)				// 预留功能 最小攻击力
				CALCUTE(m36axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p37damage)				// 预留功能 最小攻击力
				CALCUTE(m37axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p38damage)				// 预留功能 最小攻击力
				CALCUTE(m38axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p39damage)				// 预留功能 最小攻击力
				CALCUTE(m39axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p40damage)				// 预留功能 最小攻击力
				CALCUTE(m40axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p41damage)				// 预留功能 最小攻击力
				CALCUTE(m41axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p42damage)				// 预留功能 最小攻击力
				CALCUTE(m42axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p43damage)				// 预留功能 最小攻击力
				CALCUTE(m43axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p44damage)				// 预留功能 最小攻击力
				CALCUTE(m44axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p45damage)				// 预留功能 最小攻击力
				CALCUTE(m45axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p46damage)				// 预留功能 最小攻击力
				CALCUTE(m46axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p47damage)				// 预留功能 最小攻击力
				CALCUTE(m47axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p48damage)				// 预留功能 最小攻击力
				CALCUTE(m48axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p49damage)				// 预留功能 最小攻击力
				CALCUTE(m49axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p50damage)				// 预留功能 最小攻击力
				CALCUTE(m50axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p51damage)				// 预留功能 最小攻击力
				CALCUTE(m51axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p52damage)				// 预留功能 最小攻击力
				CALCUTE(m52axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p53damage)				// 预留功能 最小攻击力
				CALCUTE(m53axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p54damage)				// 预留功能 最小攻击力
				CALCUTE(m54axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p55damage)				// 预留功能 最小攻击力
				CALCUTE(m55axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p56damage)				// 预留功能 最小攻击力
				CALCUTE(m56axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p57damage)				// 预留功能 最小攻击力
				CALCUTE(m57axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p58damage)				// 预留功能 最小攻击力
				CALCUTE(m58axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p59damage)				// 预留功能 最小攻击力
				CALCUTE(m59axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p60damage)				// 预留功能 最小攻击力
				CALCUTE(m60axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p61damage)				// 预留功能 最小攻击力
				CALCUTE(m61axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p62damage)				// 预留功能 最小攻击力
				CALCUTE(m62axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p63damage)				// 预留功能 最小攻击力
				CALCUTE(m63axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p64damage)				// 预留功能 最小攻击力
				CALCUTE(m64axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p65damage)				// 预留功能 最小攻击力
				CALCUTE(m65axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p66damage)				// 预留功能 最小攻击力
				CALCUTE(m66axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p67damage)				// 预留功能 最小攻击力
				CALCUTE(m67axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p68damage)				// 预留功能 最小攻击力
				CALCUTE(m68axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p69damage)				// 预留功能 最小攻击力
				CALCUTE(m69axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p70damage)				// 预留功能 最小攻击力
				CALCUTE(m70axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p71damage)				// 预留功能 最小攻击力
				CALCUTE(m71axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p72damage)				// 预留功能 最小攻击力
				CALCUTE(m72axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p73damage)				// 预留功能 最小攻击力
				CALCUTE(m73axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p74damage)				// 预留功能 最小攻击力
				CALCUTE(m74axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p75damage)				// 预留功能 最小攻击力
				CALCUTE(m75axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p76damage)				// 预留功能 最小攻击力
				CALCUTE(m76axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p77damage)				// 预留功能 最小攻击力
				CALCUTE(m77axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p78damage)				// 预留功能 最小攻击力
				CALCUTE(m78axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p79damage)				// 预留功能 最小攻击力
				CALCUTE(m79axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p80damage)				// 预留功能 最小攻击力
				CALCUTE(m80axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p81damage)				// 预留功能 最小攻击力
				CALCUTE(m81axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p82damage)				// 预留功能 最小攻击力
				CALCUTE(m82axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p83damage)				// 预留功能 最小攻击力
				CALCUTE(m83axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p84damage)				// 预留功能 最小攻击力
				CALCUTE(m84axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p85damage)				// 预留功能 最小攻击力
				CALCUTE(m85axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p86damage)				// 预留功能 最小攻击力
				CALCUTE(m86axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p87damage)				// 预留功能 最小攻击力
				CALCUTE(m87axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p88damage)				// 预留功能 最小攻击力
				CALCUTE(m88axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p89damage)				// 预留功能 最小攻击力
				CALCUTE(m89axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p90damage)				// 预留功能 最小攻击力
				CALCUTE(m90axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p91damage)				// 预留功能 最小攻击力
				CALCUTE(m91axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p92damage)				// 预留功能 最小攻击力
				CALCUTE(m92axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p93damage)				// 预留功能 最小攻击力
				CALCUTE(m93axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p94damage)				// 预留功能 最小攻击力
				CALCUTE(m94axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p95damage)				// 预留功能 最小攻击力
				CALCUTE(m95axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p96damage)				// 预留功能 最小攻击力
				CALCUTE(m96axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p97damage)				// 预留功能 最小攻击力
				CALCUTE(m97axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p98damage)				// 预留功能 最小攻击力
				CALCUTE(m98axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p99damage)				// 预留功能 最小攻击力
				CALCUTE(m99axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p100damage)				// 预留功能 最小攻击力
				CALCUTE(m100axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p101damage)				// 预留功能 最小攻击力
				CALCUTE(m101axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p102damage)				// 预留功能 最小攻击力
				CALCUTE(m102axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p103damage)				// 预留功能 最小攻击力
				CALCUTE(m103axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p104damage)				// 预留功能 最小攻击力
				CALCUTE(m104axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p105damage)				// 预留功能 最小攻击力
				CALCUTE(m105axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p106damage)				// 预留功能 最小攻击力
				CALCUTE(m106axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p107damage)				// 预留功能 最小攻击力
				CALCUTE(m107axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p108damage)				// 预留功能 最小攻击力
				CALCUTE(m108axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p109damage)				// 预留功能 最小攻击力
				CALCUTE(m109axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p110damage)				// 预留功能 最小攻击力
				CALCUTE(m110axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p111damage)				// 预留功能 最小攻击力
				CALCUTE(m111axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p112damage)				// 预留功能 最小攻击力
				CALCUTE(m112axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p113damage)				// 预留功能 最小攻击力
				CALCUTE(m113axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p114damage)				// 预留功能 最小攻击力
				CALCUTE(m114axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p115damage)				// 预留功能 最小攻击力
				CALCUTE(m115axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p116damage)				// 预留功能 最小攻击力
				CALCUTE(m116axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p117damage)				// 预留功能 最小攻击力
				CALCUTE(m117axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p118damage)				// 预留功能 最小攻击力
				CALCUTE(m118axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p119damage)				// 预留功能 最小攻击力
				CALCUTE(m119axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p120damage)				// 预留功能 最小攻击力
				CALCUTE(m120axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p121damage)				// 预留功能 最小攻击力
				CALCUTE(m121axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p122damage)				// 预留功能 最小攻击力
				CALCUTE(m122axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p123damage)				// 预留功能 最小攻击力
				CALCUTE(m123axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p124damage)				// 预留功能 最小攻击力
				CALCUTE(m124axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p125damage)				// 预留功能 最小攻击力
				CALCUTE(m125axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p126damage)				// 预留功能 最小攻击力
				CALCUTE(m126axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p127damage)				// 预留功能 最小攻击力
				CALCUTE(m127axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p128damage)				// 预留功能 最小攻击力
				CALCUTE(m128axpdamage)			// 预留功能 最大攻击力
				CALCUTE(p129damage)				// 预留功能 最小攻击力
				CALCUTE(m129axpdamage)			// 预留功能 最大攻击力
				
				CALCUTE(m130axpdamage)			// 预留功能 最大攻击力
				
				CALCUTE(damagebonus)			// 伤害加成
			//}

			if (calcmdamage)
			{
				CALCUTE(mdamage)				// 最小法术攻击力
				CALCUTE(maxmdamage)				// 最大法术攻击力
				CALCUTE(m1damage)				// 最小法术攻击力
				CALCUTE(m1axmdamage)			// 最大法术攻击力
				CALCUTE(m2damage)				// 最小法术攻击力
				CALCUTE(m2axmdamage)			// 最大法术攻击力
				CALCUTE(m3damage)				// 斗魂最小法术攻击力
				CALCUTE(m3axmdamage)			// 斗魂最大法术攻击力
				CALCUTE(m4damage)				// 龙槽最小法术攻击力
				CALCUTE(m4axmdamage)			// 龙槽最大法术攻击力
				CALCUTE(m5damage)				// 龙星最小法术攻击力
				CALCUTE(m5axmdamage)			// 龙星最大法术攻击力
				CALCUTE(m6damage)				// 补天最小法术攻击力
				CALCUTE(m6axmdamage)			// 补天最大法术攻击力
				CALCUTE(m7damage)				// 勋章，神兵，神兽最小法术攻击力
				CALCUTE(m7axmdamage)			// 勋章，神兵，神兽最大法术攻击力
				CALCUTE(m8damage)				// 寒冰，翅膀最小法术攻击力
				CALCUTE(m8axmdamage)			// 寒冰，翅膀最大法术攻击力
				CALCUTE(m9damage)				// 神器最小法术攻击力
				CALCUTE(m9axmdamage)			// 神器最大法术攻击力
				CALCUTE(m10damage)				// 马牌精炼最小法术攻击力
				CALCUTE(m10axmdamage)			// 马牌精炼最大法术攻击力
				CALCUTE(m11damage)				// 璀璨镶嵌最小法术攻击力
				CALCUTE(m11axmdamage)			// 璀璨镶嵌最大法术攻击力
				CALCUTE(m12damage)				// 神兽宝石最小法术攻击力
				CALCUTE(m12axmdamage)			// 神兽宝石最大法术攻击力

				CALCUTE(m13damage)				// 百兽图鉴 小法术攻击力
				CALCUTE(m13axmdamage)			// 百兽图鉴 大法术攻击力
				CALCUTE(m14damage)				// 神龙镶嵌 小法术攻击力
				CALCUTE(m14axmdamage)			// 神龙镶嵌 大法术攻击力
				CALCUTE(m15damage)				// 逆天进阶 小法术攻击力
				CALCUTE(m15axmdamage)			// 逆天进阶 大法术攻击力
				CALCUTE(m16damage)				// 神魂宝石 小法术攻击力
				CALCUTE(m16axmdamage)			// 神魂宝石 大法术攻击力
				CALCUTE(m17damage)				// 暗影宝石 小法术攻击力
				CALCUTE(m17axmdamage)			// 暗影宝石 大法术攻击力
				CALCUTE(m18damage)				// 尚方宝剑 小法术攻击力
				CALCUTE(m18axmdamage)			// 尚方宝剑 大法术攻击力
				CALCUTE(m19damage)				// 紫金龙袍 小法术攻击力
				CALCUTE(m19axmdamage)			// 紫金龙袍 大法术攻击力
				CALCUTE(m20damage)				// 预留功能最 小法术攻击力
				CALCUTE(m20axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m21damage)				// 预留功能最 小法术攻击力
				CALCUTE(m21axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m22damage)				// 预留功能最 小法术攻击力
				CALCUTE(m22axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m23damage)				// 预留功能最 小法术攻击力
				CALCUTE(m23axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m24damage)				// 预留功能最 小法术攻击力
				CALCUTE(m24axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m25damage)				// 预留功能最 小法术攻击力
				CALCUTE(m25axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m26damage)				// 预留功能最 小法术攻击力
				CALCUTE(m26axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m27damage)				// 预留功能最 小法术攻击力
				CALCUTE(m27axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m28damage)				// 预留功能最 小法术攻击力
				CALCUTE(m28axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m29damage)				// 预留功能最 小法术攻击力
				CALCUTE(m29axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m30damage)				// 预留功能最 小法术攻击力
				CALCUTE(m30axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m31damage)				// 预留功能最 小法术攻击力
				CALCUTE(m31axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m32damage)				// 预留功能最 小法术攻击力
				CALCUTE(m32axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m33damage)				// 预留功能最 小法术攻击力
				CALCUTE(m33axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m34damage)				// 预留功能最 小法术攻击力
				CALCUTE(m34axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m35damage)				// 预留功能最 小法术攻击力
				CALCUTE(m35axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m36damage)				// 预留功能最 小法术攻击力
				CALCUTE(m36axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m37damage)				// 预留功能最 小法术攻击力
				CALCUTE(m37axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m38damage)				// 预留功能最 小法术攻击力
				CALCUTE(m38axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m39damage)				// 预留功能最 小法术攻击力
				CALCUTE(m39axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m40damage)				// 预留功能最 小法术攻击力
				CALCUTE(m40axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m41damage)				// 预留功能最 小法术攻击力
				CALCUTE(m41axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m42damage)				// 预留功能最 小法术攻击力
				CALCUTE(m42axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m43damage)				// 预留功能最 小法术攻击力
				CALCUTE(m43axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m44damage)				// 预留功能最 小法术攻击力
				CALCUTE(m44axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m45damage)				// 预留功能最 小法术攻击力
				CALCUTE(m45axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m46damage)				// 预留功能最 小法术攻击力
				CALCUTE(m46axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m47damage)				// 预留功能最 小法术攻击力
				CALCUTE(m47axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m48damage)				// 预留功能最 小法术攻击力
				CALCUTE(m48axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m49damage)				// 预留功能最 小法术攻击力
				CALCUTE(m49axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m50damage)				// 预留功能最 小法术攻击力
				CALCUTE(m50axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m51damage)				// 预留功能最 小法术攻击力
				CALCUTE(m51axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m52damage)				// 预留功能最 小法术攻击力
				CALCUTE(m52axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m53damage)				// 预留功能最 小法术攻击力
				CALCUTE(m53axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m54damage)				// 预留功能最 小法术攻击力
				CALCUTE(m54axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m55damage)				// 预留功能最 小法术攻击力
				CALCUTE(m55axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m56damage)				// 预留功能最 小法术攻击力
				CALCUTE(m56axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m57damage)				// 预留功能最 小法术攻击力
				CALCUTE(m57axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m58damage)				// 预留功能最 小法术攻击力
				CALCUTE(m58axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m59damage)				// 预留功能最 小法术攻击力
				CALCUTE(m59axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m60damage)				// 预留功能最 小法术攻击力
				CALCUTE(m60axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m61damage)				// 预留功能最 小法术攻击力
				CALCUTE(m61axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m62damage)				// 预留功能最 小法术攻击力
				CALCUTE(m62axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m63damage)				// 预留功能最 小法术攻击力
				CALCUTE(m63axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m64damage)				// 预留功能最 小法术攻击力
				CALCUTE(m64axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m65damage)				// 预留功能最 小法术攻击力
				CALCUTE(m65axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m66damage)				// 预留功能最 小法术攻击力
				CALCUTE(m66axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m67damage)				// 预留功能最 小法术攻击力
				CALCUTE(m67axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m68damage)				// 预留功能最 小法术攻击力
				CALCUTE(m68axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m69damage)				// 预留功能最 小法术攻击力
				CALCUTE(m69axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m70damage)				// 预留功能最 小法术攻击力
				CALCUTE(m70axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m71damage)				// 预留功能最 小法术攻击力
				CALCUTE(m71axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m72damage)				// 预留功能最 小法术攻击力
				CALCUTE(m72axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m73damage)				// 预留功能最 小法术攻击力
				CALCUTE(m73axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m74damage)				// 预留功能最 小法术攻击力
				CALCUTE(m74axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m75damage)				// 预留功能最 小法术攻击力
				CALCUTE(m75axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m76damage)				// 预留功能最 小法术攻击力
				CALCUTE(m76axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m77damage)				// 预留功能最 小法术攻击力
				CALCUTE(m77axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m78damage)				// 预留功能最 小法术攻击力
				CALCUTE(m78axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m79damage)				// 预留功能最 小法术攻击力
				CALCUTE(m79axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m80damage)				// 预留功能最 小法术攻击力
				CALCUTE(m80axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m81damage)				// 预留功能最 小法术攻击力
				CALCUTE(m81axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m82damage)				// 预留功能最 小法术攻击力
				CALCUTE(m82axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m83damage)				// 预留功能最 小法术攻击力
				CALCUTE(m83axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m84damage)				// 预留功能最 小法术攻击力
				CALCUTE(m84axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m85damage)				// 预留功能最 小法术攻击力
				CALCUTE(m85axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m86damage)				// 预留功能最 小法术攻击力
				CALCUTE(m86axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m87damage)				// 预留功能最 小法术攻击力
				CALCUTE(m87axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m88damage)				// 预留功能最 小法术攻击力
				CALCUTE(m88axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m89damage)				// 预留功能最 小法术攻击力
				CALCUTE(m89axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m90damage)				// 预留功能最 小法术攻击力
				CALCUTE(m90axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m91damage)				// 预留功能最 小法术攻击力
				CALCUTE(m91axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m92damage)				// 预留功能最 小法术攻击力
				CALCUTE(m92axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m93damage)				// 预留功能最 小法术攻击力
				CALCUTE(m93axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m94damage)				// 预留功能最 小法术攻击力
				CALCUTE(m94axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m95damage)				// 预留功能最 小法术攻击力
				CALCUTE(m95axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m96damage)				// 预留功能最 小法术攻击力
				CALCUTE(m96axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m97damage)				// 预留功能最 小法术攻击力
				CALCUTE(m97axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m98damage)				// 预留功能最 小法术攻击力
				CALCUTE(m98axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m99damage)				// 预留功能最 小法术攻击力
				CALCUTE(m99axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m100damage)				// 预留功能最 小法术攻击力
				CALCUTE(m100axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m101damage)				// 预留功能最 小法术攻击力
				CALCUTE(m101axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m102damage)				// 预留功能最 小法术攻击力
				CALCUTE(m102axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m103damage)				// 预留功能最 小法术攻击力
				CALCUTE(m103axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m104damage)				// 预留功能最 小法术攻击力
				CALCUTE(m104axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m105damage)				// 预留功能最 小法术攻击力
				CALCUTE(m105axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m106damage)				// 预留功能最 小法术攻击力
				CALCUTE(m106axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m107damage)				// 预留功能最 小法术攻击力
				CALCUTE(m107axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m108damage)				// 预留功能最 小法术攻击力
				CALCUTE(m108axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m109damage)				// 预留功能最 小法术攻击力
				CALCUTE(m109axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m110damage)				// 预留功能最 小法术攻击力
				CALCUTE(m110axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m111damage)				// 预留功能最 小法术攻击力
				CALCUTE(m111axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m112damage)				// 预留功能最 小法术攻击力
				CALCUTE(m112axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m113damage)				// 预留功能最 小法术攻击力
				CALCUTE(m113axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m114damage)				// 预留功能最 小法术攻击力
				CALCUTE(m114axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m115damage)				// 预留功能最 小法术攻击力
				CALCUTE(m115axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m116damage)				// 预留功能最 小法术攻击力
				CALCUTE(m116axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m117damage)				// 预留功能最 小法术攻击力
				CALCUTE(m117axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m118damage)				// 预留功能最 小法术攻击力
				CALCUTE(m118axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m119damage)				// 预留功能最 小法术攻击力
				CALCUTE(m119axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m120damage)				// 预留功能最 小法术攻击力
				CALCUTE(m120axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m121damage)				// 预留功能最 小法术攻击力
				CALCUTE(m121axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m122damage)				// 预留功能最 小法术攻击力
				CALCUTE(m122axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m123damage)				// 预留功能最 小法术攻击力
				CALCUTE(m123axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m124damage)				// 预留功能最 小法术攻击力
				CALCUTE(m124axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m125damage)				// 预留功能最 小法术攻击力
				CALCUTE(m125axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m126damage)				// 预留功能最 小法术攻击力
				CALCUTE(m126axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m127damage)				// 预留功能最 小法术攻击力
				CALCUTE(m127axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m128damage)				// 预留功能最 小法术攻击力
				CALCUTE(m128axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m129damage)				// 预留功能最 小法术攻击力
				CALCUTE(m129axmdamage)			// 预留功能最 大法术攻击力
				CALCUTE(m130damage)				// 预留功能最 小法术攻击力
				CALCUTE(m130axmdamage)			// 预留功能最 大法术攻击力
				
			}
			else
			{
				equips.appendminpet+=container[i]->data.mdamage;
				equips.appendmaxpet+=container[i]->data.maxmdamage;
			}
		
			CALCUTE(pdefence)				// 物防
			CALCUTE(mdefence)				// 魔防
			CALCUTE(p1defence)				// 物防
			CALCUTE(m1defence)				// 魔防
			CALCUTE(p2defence)				// 物防
			CALCUTE(m2defence)				// 魔防
			CALCUTE(p3defence)				// 斗魂物防
			CALCUTE(m3defence)				// 斗魂魔防
			CALCUTE(p4defence)				// 龙槽物防
			CALCUTE(m4defence)				// 龙槽魔防
			CALCUTE(p5defence)				// 龙星物防
			CALCUTE(m5defence)				// 龙星魔防
			CALCUTE(p6defence)				// 补天物防
			CALCUTE(m6defence)				// 补天魔防
			CALCUTE(p7defence)				// 勋章，神兵，神兽物防
			CALCUTE(m7defence)				// 勋章，神兵，神兽魔防
			CALCUTE(p8defence)				// 寒冰，翅膀物防
			CALCUTE(m8defence)				// 寒冰，翅膀魔防
			CALCUTE(p9defence)				// 神器物防
			CALCUTE(m9defence)				// 神器魔防
			CALCUTE(p10defence)				// 马牌精炼物防
			CALCUTE(m10defence)				// 马牌精炼魔防
			CALCUTE(p11defence)				// 璀璨镶嵌物防
			CALCUTE(m11defence)				// 璀璨镶嵌魔防
			CALCUTE(p12defence)				// 神兽宝石物防
			CALCUTE(m12defence)				// 神兽宝石魔防
			
			CALCUTE(p13defence)				// 百兽图鉴 物防
			CALCUTE(m13defence)				// 百兽图鉴 魔防
			CALCUTE(p14defence)				// 神龙镶嵌 物防
			CALCUTE(m14defence)				// 神龙镶嵌 魔防
			CALCUTE(p15defence)				// 逆天进阶 物防
			CALCUTE(m15defence)				// 逆天进阶 魔防
			CALCUTE(p16defence)				// 神魂宝石 物防
			CALCUTE(m16defence)				// 神魂宝石 魔防
			CALCUTE(p17defence)				// 暗影宝石 物防
			CALCUTE(m17defence)				// 暗影宝石 魔防
			CALCUTE(p18defence)				// 尚方宝剑 物防
			CALCUTE(m18defence)				// 尚方宝剑 魔防
			CALCUTE(p19defence)				// 紫金龙袍 物防
			CALCUTE(m19defence)				// 紫金龙袍 魔防
			CALCUTE(p20defence)				// 预留功能 物防
			CALCUTE(m20defence)				// 预留功能 魔防
			CALCUTE(p21defence)				// 预留功能 物防
			CALCUTE(m21defence)				// 预留功能 魔防
			CALCUTE(p22defence)				// 预留功能 物防
			CALCUTE(m22defence)				// 预留功能 魔防
			CALCUTE(p23defence)				// 预留功能 物防
			CALCUTE(m23defence)				// 预留功能 魔防
			CALCUTE(p24defence)				// 预留功能 物防
			CALCUTE(m24defence)				// 预留功能 魔防
			CALCUTE(p25defence)				// 预留功能 物防
			CALCUTE(m25defence)				// 预留功能 魔防
			CALCUTE(p26defence)				// 预留功能 物防
			CALCUTE(m26defence)				// 预留功能 魔防
			CALCUTE(p27defence)				// 预留功能 物防
			CALCUTE(m27defence)				// 预留功能 魔防
			CALCUTE(p28defence)				// 预留功能 物防
			CALCUTE(m28defence)				// 预留功能 魔防
			CALCUTE(p29defence)				// 预留功能 物防
			CALCUTE(m29defence)				// 预留功能 魔防
			CALCUTE(p30defence)				// 预留功能 物防
			CALCUTE(m30defence)				// 预留功能 魔防
			CALCUTE(p31defence)				// 预留功能 物防
			CALCUTE(m31defence)				// 预留功能 魔防
			CALCUTE(p32defence)				// 预留功能 物防
			CALCUTE(m32defence)				// 预留功能 魔防
			CALCUTE(p33defence)				// 预留功能 物防
			CALCUTE(m33defence)				// 预留功能 魔防
			CALCUTE(p34defence)				// 预留功能 物防
			CALCUTE(m34defence)				// 预留功能 魔防
			CALCUTE(p35defence)				// 预留功能 物防
			CALCUTE(m35defence)				// 预留功能 魔防
			CALCUTE(p36defence)				// 预留功能 物防
			CALCUTE(m36defence)				// 预留功能 魔防
			CALCUTE(p37defence)				// 预留功能 物防
			CALCUTE(m37defence)				// 预留功能 魔防
			CALCUTE(p38defence)				// 预留功能 物防
			CALCUTE(m38defence)				// 预留功能 魔防
			CALCUTE(p39defence)				// 预留功能 物防
			CALCUTE(m39defence)				// 预留功能 魔防
			CALCUTE(p40defence)				// 预留功能 物防
			CALCUTE(m40defence)				// 预留功能 魔防
			CALCUTE(p41defence)				// 预留功能 物防
			CALCUTE(m41defence)				// 预留功能 魔防
			CALCUTE(p42defence)				// 预留功能 物防
			CALCUTE(m42defence)				// 预留功能 魔防
			CALCUTE(p43defence)				// 预留功能 物防
			CALCUTE(m43defence)				// 预留功能 魔防
			CALCUTE(p44defence)				// 预留功能 物防
			CALCUTE(m44defence)				// 预留功能 魔防
			CALCUTE(p45defence)				// 预留功能 物防
			CALCUTE(m45defence)				// 预留功能 魔防
			CALCUTE(p46defence)				// 预留功能 物防
			CALCUTE(m46defence)				// 预留功能 魔防
			CALCUTE(p47defence)				// 预留功能 物防
			CALCUTE(m47defence)				// 预留功能 魔防
			CALCUTE(p48defence)				// 预留功能 物防
			CALCUTE(m48defence)				// 预留功能 魔防
			CALCUTE(p49defence)				// 预留功能 物防
			CALCUTE(m49defence)				// 预留功能 魔防
			CALCUTE(p50defence)				// 预留功能 物防
			CALCUTE(m50defence)				// 预留功能 魔防
			CALCUTE(p51defence)				// 预留功能 物防
			CALCUTE(m51defence)				// 预留功能 魔防
			CALCUTE(p52defence)				// 预留功能 物防
			CALCUTE(m52defence)				// 预留功能 魔防
			CALCUTE(p53defence)				// 预留功能 物防
			CALCUTE(m53defence)				// 预留功能 魔防
			CALCUTE(p54defence)				// 预留功能 物防
			CALCUTE(m54defence)				// 预留功能 魔防
			CALCUTE(p55defence)				// 预留功能 物防
			CALCUTE(m55defence)				// 预留功能 魔防
			CALCUTE(p56defence)				// 预留功能 物防
			CALCUTE(m56defence)				// 预留功能 魔防
			CALCUTE(p57defence)				// 预留功能 物防
			CALCUTE(m57defence)				// 预留功能 魔防
			CALCUTE(p58defence)				// 预留功能 物防
			CALCUTE(m58defence)				// 预留功能 魔防
			CALCUTE(p59defence)				// 预留功能 物防
			CALCUTE(m59defence)				// 预留功能 魔防
			CALCUTE(p60defence)				// 预留功能 物防
			CALCUTE(m60defence)				// 预留功能 魔防
			CALCUTE(p61defence)				// 预留功能 物防
			CALCUTE(m61defence)				// 预留功能 魔防
			CALCUTE(p62defence)				// 预留功能 物防
			CALCUTE(m62defence)				// 预留功能 魔防
			CALCUTE(p63defence)				// 预留功能 物防
			CALCUTE(m63defence)				// 预留功能 魔防
			CALCUTE(p64defence)				// 预留功能 物防
			CALCUTE(m64defence)				// 预留功能 魔防
			CALCUTE(p65defence)				// 预留功能 物防
			CALCUTE(m65defence)				// 预留功能 魔防
			CALCUTE(p66defence)				// 预留功能 物防
			CALCUTE(m66defence)				// 预留功能 魔防
			CALCUTE(p67defence)				// 预留功能 物防
			CALCUTE(m67defence)				// 预留功能 魔防
			CALCUTE(p68defence)				// 预留功能 物防
			CALCUTE(m68defence)				// 预留功能 魔防
			CALCUTE(p69defence)				// 预留功能 物防
			CALCUTE(m69defence)				// 预留功能 魔防
			CALCUTE(p70defence)				// 预留功能 物防
			CALCUTE(m70defence)				// 预留功能 魔防
			CALCUTE(p71defence)				// 预留功能 物防
			CALCUTE(m71defence)				// 预留功能 魔防
			CALCUTE(p72defence)				// 预留功能 物防
			CALCUTE(m72defence)				// 预留功能 魔防
			CALCUTE(p73defence)				// 预留功能 物防
			CALCUTE(m73defence)				// 预留功能 魔防
			CALCUTE(p74defence)				// 预留功能 物防
			CALCUTE(m74defence)				// 预留功能 魔防
			CALCUTE(p75defence)				// 预留功能 物防
			CALCUTE(m75defence)				// 预留功能 魔防
			CALCUTE(p76defence)				// 预留功能 物防
			CALCUTE(m76defence)				// 预留功能 魔防
			CALCUTE(p77defence)				// 预留功能 物防
			CALCUTE(m77defence)				// 预留功能 魔防
			CALCUTE(p78defence)				// 预留功能 物防
			CALCUTE(m78defence)				// 预留功能 魔防
			CALCUTE(p79defence)				// 预留功能 物防
			CALCUTE(m79defence)				// 预留功能 魔防
			CALCUTE(p80defence)				// 预留功能 物防
			CALCUTE(m80defence)				// 预留功能 魔防
			CALCUTE(p81defence)				// 预留功能 物防
			CALCUTE(m81defence)				// 预留功能 魔防
			CALCUTE(p82defence)				// 预留功能 物防
			CALCUTE(m82defence)				// 预留功能 魔防
			CALCUTE(p83defence)				// 预留功能 物防
			CALCUTE(m83defence)				// 预留功能 魔防
			CALCUTE(p84defence)				// 预留功能 物防
			CALCUTE(m84defence)				// 预留功能 魔防
			CALCUTE(p85defence)				// 预留功能 物防
			CALCUTE(m85defence)				// 预留功能 魔防
			CALCUTE(p86defence)				// 预留功能 物防
			CALCUTE(m86defence)				// 预留功能 魔防
			CALCUTE(p87defence)				// 预留功能 物防
			CALCUTE(m87defence)				// 预留功能 魔防
			CALCUTE(p88defence)				// 预留功能 物防
			CALCUTE(m88defence)				// 预留功能 魔防
			CALCUTE(p89defence)				// 预留功能 物防
			CALCUTE(m89defence)				// 预留功能 魔防
			CALCUTE(p90defence)				// 预留功能 物防
			CALCUTE(m90defence)				// 预留功能 魔防
			CALCUTE(p91defence)				// 预留功能 物防
			CALCUTE(m91defence)				// 预留功能 魔防
			CALCUTE(p92defence)				// 预留功能 物防
			CALCUTE(m92defence)				// 预留功能 魔防
			CALCUTE(p93defence)				// 预留功能 物防
			CALCUTE(m93defence)				// 预留功能 魔防
			CALCUTE(p94defence)				// 预留功能 物防
			CALCUTE(m94defence)				// 预留功能 魔防
			CALCUTE(p95defence)				// 预留功能 物防
			CALCUTE(m95defence)				// 预留功能 魔防
			CALCUTE(p96defence)				// 预留功能 物防
			CALCUTE(m96defence)				// 预留功能 魔防
			CALCUTE(p97defence)				// 预留功能 物防
			CALCUTE(m97defence)				// 预留功能 魔防
			CALCUTE(p98defence)				// 预留功能 物防
			CALCUTE(m98defence)				// 预留功能 魔防
			CALCUTE(p99defence)				// 预留功能 物防
			CALCUTE(m99defence)				// 预留功能 魔防
			CALCUTE(p100defence)			// 预留功能 物防
			CALCUTE(m100defence)			// 预留功能 魔防
			CALCUTE(p101defence)			// 预留功能 物防
			CALCUTE(m101defence)			// 预留功能 魔防
			CALCUTE(p102defence)			// 预留功能 物防
			CALCUTE(m102defence)			// 预留功能 魔防
			CALCUTE(p103defence)			// 预留功能 物防
			CALCUTE(m103defence)			// 预留功能 魔防
			CALCUTE(p104defence)			// 预留功能 物防
			CALCUTE(m104defence)			// 预留功能 魔防
			CALCUTE(p105defence)			// 预留功能 物防
			CALCUTE(m105defence)			// 预留功能 魔防
			CALCUTE(p106defence)			// 预留功能 物防
			CALCUTE(m106defence)			// 预留功能 魔防
			CALCUTE(p107defence)			// 预留功能 物防
			CALCUTE(m107defence)			// 预留功能 魔防
			CALCUTE(p108defence)			// 预留功能 物防
			CALCUTE(m108defence)			// 预留功能 魔防
			CALCUTE(p109defence)			// 预留功能 物防
			CALCUTE(m109defence)			// 预留功能 魔防
			CALCUTE(p110defence)			// 预留功能 物防
			CALCUTE(m110defence)			// 预留功能 魔防
			CALCUTE(p111defence)			// 预留功能 物防
			CALCUTE(m111defence)			// 预留功能 魔防
			CALCUTE(p112defence)			// 预留功能 物防
			CALCUTE(m112defence)			// 预留功能 魔防
			CALCUTE(p113defence)			// 预留功能 物防
			CALCUTE(m113defence)			// 预留功能 魔防
			CALCUTE(p114defence)			// 预留功能 物防
			CALCUTE(m114defence)			// 预留功能 魔防
			CALCUTE(p115defence)			// 预留功能 物防
			CALCUTE(m115defence)			// 预留功能 魔防
			CALCUTE(p116defence)			// 预留功能 物防
			CALCUTE(m116defence)			// 预留功能 魔防
			CALCUTE(p117defence)			// 预留功能 物防
			CALCUTE(m117defence)			// 预留功能 魔防
			CALCUTE(p118defence)			// 预留功能 物防
			CALCUTE(m118defence)			// 预留功能 魔防
			CALCUTE(p119defence)			// 预留功能 物防
			CALCUTE(m119defence)			// 预留功能 魔防
			CALCUTE(p120defence)			// 预留功能 物防
			CALCUTE(m120defence)			// 预留功能 魔防
			CALCUTE(p121defence)			// 预留功能 物防
			CALCUTE(m121defence)			// 预留功能 魔防
			CALCUTE(p122defence)			// 预留功能 物防
			CALCUTE(m122defence)			// 预留功能 魔防
			CALCUTE(p123defence)			// 预留功能 物防
			CALCUTE(m123defence)			// 预留功能 魔防
			CALCUTE(p124defence)			// 预留功能 物防
			CALCUTE(m124defence)			// 预留功能 魔防
			CALCUTE(p125defence)			// 预留功能 物防
			CALCUTE(m125defence)			// 预留功能 魔防
			CALCUTE(p126defence)			// 预留功能 物防
			CALCUTE(m126defence)			// 预留功能 魔防
			CALCUTE(p127defence)			// 预留功能 物防
			CALCUTE(m127defence)			// 预留功能 魔防
			CALCUTE(p128defence)			// 预留功能 物防
			CALCUTE(m128defence)			// 预留功能 魔防
			CALCUTE(p129defence)			// 预留功能 物防
			CALCUTE(m129defence)			// 预留功能 魔防
			CALCUTE(p130defence)			// 预留功能 物防
			CALCUTE(m130defence)			// 预留功能 魔防

			CALCUTE(hongtiannum1)			// 预留功能 物防
			CALCUTE(hongtiannum2)			// 预留功能 魔防
			CALCUTE(hongtiannum3)			// 预留功能 魔防
			CALCUTE(hongtiannum4)			// 预留功能 魔防
			CALCUTE(hongtiannum5)			// 预留功能 魔防
			CALCUTE(hongtiannum6)			// 预留功能 魔防
			CALCUTE(hongtiannum7)			// 预留功能 魔防
			CALCUTE(hongtiannum8)			// 预留功能 最大生命值
			CALCUTE(hongtiannum9)			// 预留功能 最小攻击力

			CALCUTE(damage)					// 增加伤害值x％
			
			CALCUTE(akspeed)				// 攻击速度
			CALCUTE(mvspeed)				// 移动速度
			CALCUTE(atrating)				// 命中率
			CALCUTE(akdodge)				// 躲避率
		
			CALCUTE(str)				  	// 力量
			CALCUTE(inte)  					// 智力
			CALCUTE(dex)  					// 敏捷
			CALCUTE(spi)  					// 精神
			CALCUTE(con)  					// 体质

			CALCUTE(xstr)				  	// 力量-魂魄
			CALCUTE(xinte)  				// 智力-魂魄
			CALCUTE(xdex)  					// 敏捷-魂魄
			CALCUTE(xspi)  					// 精神-魂魄
			CALCUTE(xcon)  					// 体质-魂魄
			
			CALCUTE(hpr)					// 生命值恢复
			CALCUTE(mpr)  					// 法术值恢复
			CALCUTE(spr)  					// 体力值恢复
			
			CALCUTE(holy) 					//神圣一击
			CALCUTE(bang) 					//重击
			CALCUTE(pdam) 					// 增加物理攻击力
			CALCUTE(pdef)  					// 增加物理防御力
			CALCUTE(mdam)  					// 增加魔法攻击力
			CALCUTE(mdef)  					// 增加魔法防御力
			
			CALCUTE(poisondef) 				//抗毒增加
			CALCUTE(lulldef) 				//抗麻痹增加
			CALCUTE(reeldef) 				//抗眩晕增加
			CALCUTE(evildef) 				//抗噬魔增加
			CALCUTE(bitedef) 				//抗噬力增加
			CALCUTE(chaosdef) 				//抗混乱增加
			CALCUTE(colddef) 				//抗冰冻增加
			CALCUTE(petrifydef) 			//抗石化增加
			CALCUTE(blinddef) 				//抗失明增加
			CALCUTE(stabledef) 				//抗定身增加
			CALCUTE(slowdef)				 //抗减速增加
			CALCUTE(luredef) 				//抗诱惑增加
			
			CALCUTE(poison) 				//中毒增加
			CALCUTE(lull) 					//麻痹增加
			CALCUTE(reel) 					//眩晕增加
			CALCUTE(evil) 					//噬魔增加
			CALCUTE(bite)					//噬力增加
			CALCUTE(chaos) 					//混乱增加
			CALCUTE(cold) 					//冰冻增加
			CALCUTE(petrify) 				//石化增加
			CALCUTE(blind) 					//失明增加
			CALCUTE(stable) 				//定身增加
			CALCUTE(slow) 					//减速增加
			CALCUTE(lure) 					//诱惑增加
			CALCUTE(hpleech.odds) 
			CALCUTE(hpleech.effect) 
			CALCUTE(mpleech.odds) 			//x%吸收生命值y
			CALCUTE(mpleech.effect) 		//x%吸收法术值y

			CALCUTE(hptomp)					//转换生命值为法术值x％
			CALCUTE(dhpp) 					//物理伤害减少x%	
			CALCUTE(dmpp)					//法术伤害值减少x%		
		
			CALCUTE(incgold)				//增加银子掉落x%
			CALCUTE(doublexp)				//x%双倍经验			
			if (container[i]->base->kind == ItemType_DoubleExp) 
			{
				equips.doublexp=100;				//x%双倍经验		
			}
			CALCUTE(mf)						 //增加掉宝率x%

			switch ( container[i]->base->kind) 
			{
				case ItemType_Blade:		    //104代表武术刀类武器
				case ItemType_Sword :	        //105代表武术剑类武器
				case ItemType_Axe:	           //106代表武术斧类武器
				case ItemType_Hammer:	        //107代表武术斧类武器
				case ItemType_Staff:		    //108代表法术杖类武器
				case ItemType_Crossbow:	        //109代表箭术弓类武器
				case ItemType_Fan:	            //110代表美女扇类
				case ItemType_Stick:	        //111代表召唤棍类武器
				case ItemType_Dagger:	        //136代表利刃
			    case ItemType_NewSword:	        //137代表卫士剑
				case ItemType_LongGun:          //155代表长枪
					equips.aftype = container[i]->data.fivetype;
					equips.afpoint = container[i]->data.fivepoint;

					break;
				case ItemType_Necklace:	        //117代表角色项链类
				case ItemType_Fing:		        //118代表角色戒指类
				case ItemType_HorseShoe:        //130代表坐骑马脚类
			    case ItemType_HorseSafe:        //133代表坐骑马甲类
			    case ItemType_HorseIron:        //134代表坐骑马镫类
					//note: be careful, weapon must be computed before this
					if (container[i]->data.fivetype == equips.aftype) 
					{
						equips.afpoint += container[i]->data.fivepoint;
					}
					break;
				case ItemType_ClothBody:		//101代表布质加生命类服装
				case ItemType_FellBody:		    //102代表皮甲加魔防类服装
				case ItemType_MetalBody:		//103代表金属铠甲加物防类服装
					equips.dftype = container[i]->data.fivetype;
					equips.dfpoint = container[i]->data.fivepoint;

					break;
				case ItemType_Shield:	        //112代表盾牌类
				case ItemType_Helm:		        //113代表角色头盔类
				case ItemType_Caestus:	        //114代表角色腰带类
				case ItemType_Cuff:		        //115代表角色护腕类
				case ItemType_Shoes:		    //116代表角色鞋子类
			    case ItemType_HorseRope:        //131代表坐骑马甲类
			    case ItemType_HorseSaddle:      //132代表坐骑马镫类
				case ItemType_breastplate:	    //138代表护心镜
	        	case ItemType_StaffFlag:		//141代表战旗(法师)
	        	case ItemType_CrossFlag:	    //142代表战旗(弓手)
	        	case ItemType_Flag:	            //143代表战旗(仙术)
	        	case ItemType_StickFlag:	    //144代表战旗(召唤)
	        	case ItemType_BladeFlag:	    //145代表战旗(战士)
	       	 	case ItemType_DaggerFlag:	    //146代表战旗(刺客)
	        	case ItemType_SwordFlag:        //147代表战旗(卫士)
				case ItemType_Handgun:          //156代表短枪
					//note: be careful, armor must be computed before this
					if (container[i]->data.fivetype == equips.dftype) 
					{
						equips.dfpoint += container[i]->data.fivepoint;
					}
					break;
			}
			
			int j = 0;
		
			while (j<10 && container[i]->data.skill[j].id && equips.getMaxSkill(container[i]->data.skill[j].id) < container[i]->data.skill[j].point) 
			{
				equips.skill[container[i]->data.skill[j].id] = container[i]->data.skill[j].point;
				++j;
			}

			if (container[i]->data.skills.id && equips.getMaxSkill(container[i]->data.skills.id) < container[i]->data.skills.point) 
			{
				equips.skills[container[i]->data.skills.id] = container[i]->data.skills.point;
			}
			
			int index = 0;
			int found[16];
			memset(found, 0, sizeof(int)*16);
			
			if (container[i]->data.fivetype != FIVE_NONE) 
			{
				for (int j=0; j<16; ++j)	
				{
					//////////////////////////////////////
					//soke 装备的灵魂效果
					if ( i!=j && container[j]!=NULL && container[j]->data.dur != 0 && container[i]->data.fiveset[index]==container[j]->data.dwObjectID  && !found[j] &&
						( ((container[i]->data.fivetype+3*(index+1))%5) == container[j]->data.fivetype) ) 
						{				
/*						
							switch (index) 
							{
								case 0:
									CALCUTE(dpdam)			//物理伤害减少%x
									break;
								case 1:
									CALCUTE(dmdam)			//法术伤害减少%x
									break;
								case 2:
									CALCUTE(bdam)			//增加伤害x%
									break;
								case 3:
									CALCUTE(rdam)			//伤害反射%x
									break;
								case 4:
									CALCUTE(ignoredef)		//%x忽视目标防御
									break;
								case 5:
									CALCUTE(addpdef)        //物理防御力增强600
									break;
								case 6:
									CALCUTE(addmdef)        //魔法防御力增强600
									break;
								case 7:
									CALCUTE(addpdam)        //增加物理攻击力1200
									break;
								case 8:
									CALCUTE(addmdam)        //增加魔法攻击力1200
									break;
								case 9:
									CALCUTE(addhp)          //增加生命值10000
									break;
							}
*/
							int k = 0;
							while (k < 5) 
							{
								if (container[i]->data._five_props[k] ) 
								{
									if (k>=index) break;
								}
								++k;
							}
							equips._five_props[k] += container[i]->data._five_props[k];
	                           
							++index;
							found[j] = 1;
							j = 0; //loop again
					}
				}				
			}
		}
	}
	switch(suitnum.first)
	{
		case 0:
			{//soke 这里是设置装备灵魂激活的效果
				if(suitnum.second >= 10)
				{
					equips.pdam+=5; 					// 增加物理攻击力
					equips.pdef+=5;  					// 增加物理防御力
					equips.mdam+=5;  					// 增加魔法攻击力
					equips.mdef+=5;  					// 增加魔法防御力
					equips.maxhprate +=8;
					equips.maxmprate +=8;
				}
				else
				{
					equips.pdam+=2; 					// 增加物理攻击力
					equips.pdef+=2;  					// 增加物理防御力
					equips.mdam+=2;  					// 增加魔法攻击力
					equips.mdef+=2;  					// 增加魔法防御力
					equips.maxhprate +=4;
					equips.maxmprate +=4;
				}
			}
			break;
		case 1:
			{
				if(suitnum.second >= 10)
				{
					equips.pdam+=5; 					// 增加物理攻击力
					equips.pdef+=5;  					// 增加物理防御力
					equips.mdam+=5;  					// 增加魔法攻击力
					equips.mdef+=5;  					// 增加魔法防御力
					equips.maxhprate +=8;
					equips.maxmprate +=8;
				}
				else
				{
					equips.pdam+=2; 					// 增加物理攻击力
					equips.pdef+=2;  					// 增加物理防御力
					equips.mdam+=2;  					// 增加魔法攻击力
					equips.mdef+=2;  					// 增加魔法防御力
					equips.maxhprate +=4;
					equips.maxmprate +=4;
				}
			}
			break;
		case 2:
			{
				if(suitnum.second >= 10)
				{
					equips.pdam+=5; 					// 增加物理攻击力
					equips.pdef+=5;  					// 增加物理防御力
					equips.mdam+=5;  					// 增加魔法攻击力
					equips.mdef+=5;  					// 增加魔法防御力
					equips.maxhprate +=8;
					equips.maxmprate +=8;
				}
				else
				{
					equips.pdam+=2; 					// 增加物理攻击力
					equips.pdef+=2;  					// 增加物理防御力
					equips.mdam+=2;  					// 增加魔法攻击力
					equips.mdef+=2;  					// 增加魔法防御力
					equips.maxhprate +=4;
					equips.maxmprate +=4;
				}
			}
			break;
		default:
			break;
	}
	//Zebra::logger->debug("装备攻击五行属性(%d:%d)，防御五行(%d:%d)", equips.aftype, equips.afpoint, equips.dftype, equips.dfpoint);
}

/*

{ 	-1, 	1, 	0, 	3, 	2, 	5, 	8, 	6, 	4, 	10,
	11, 	16,	18,	13,	-1,	15,	9,	7,	-1,	-1,
	12, 	17,	19,	14,	-1,	-1,	-1,	-1,	-1,	-1,
	13, 	31,	32,	33,	34,	35,	36,	37,	38,	39,
	14, 	41,	42,	43,	-1,	-1,	-1,	-1,	-1,	-1,
	15, 	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1};

*/

int EquipPack::position(WORD x, WORD y) const
{
	int pos = x*10 + y;
	if (pos < (int)sizeof(_poses) && _poses[pos] != -1) {
		//Zebra::logger->debug("x:%d_y:%d_pos:%d",x,y,_poses[pos]);
		return _poses[pos];
	}

	return -1;
}		

//soke 身上穿上装备
bool EquipPack::process_extra_add(zObject* ob)
{
	if (ob && (ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_MAKE || ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_PACKAGE)) 
	{
		int pos = ((ob->data.pos.ypos() - Cmd::EQUIPCELLTYPE_PACKAGE) << 1)  + ob->data.pos.xpos();
		assert(pos >= 0 && pos <10);
		SAFE_DELETE(packs[pos]);
		packs[pos] = new ObjectPack(ob, ob->data.pos.ypos() -1 );
		return true;
	}

	return false;
}

//soke 身上已装备的装备移除
bool EquipPack::process_extra_remove(zObject* ob)
{
	if (ob && (ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_MAKE || ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_PACKAGE)) 
	{
		int pos = ((ob->data.pos.ypos() - Cmd::EQUIPCELLTYPE_PACKAGE) << 1)  + ob->data.pos.xpos();
		assert(pos >= 0 && pos <10);
		SAFE_DELETE(packs[pos]);
		return true;
	}

	return false;
}

bool EquipPack::process_extra_get(zObject** ob, WORD x, WORD y)
{
	if (y == Cmd::EQUIPCELLTYPE_MAKE ||y == Cmd::EQUIPCELLTYPE_PACKAGE) 
	{
		int pos = ((y - Cmd::EQUIPCELLTYPE_PACKAGE) << 1)  + x;
		if (pos<4) 
		{
			if (packs[pos] && packs[pos]->object() ) *ob = packs[pos]->object();
			return true;
		}
	}

	return false;
}

ObjectPack* EquipPack::pack(PACKNO no) const
{
	return packs[no];
}

zObject* EquipPack::equip(EQUIPNO no) const
{
	return container[no];
}





//soke 物品装备的位置
// const int EquipPack::_poses[] = {   -1,  1,  0,  3,  2,  5,  8,  6,  4,  10,
// 11,  16,  18,  13,  14,  11,  9,  7,  18,  19,
// 12,  17,  19,  14,  -1,  -1,  -1,  -1,  -1,  -1,
// 13,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
// 14,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
// 15,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};
/*
int pos = x*20 + y;
//soke 征途原版
const int EquipPack::_poses[] = { 	-1, 	1, 	0, 	3, 	2, 	5, 	8, 	6, 	4, 	10,
									11, 	16,	18,	13,	-1,	-1,	9,	7,	-1,	-1,
									12, 	17,	19,	14,	-1,	-1,	-1,	-1,	-1,	-1,
									13, 	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
									14, 	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
									15, 	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1}; */
									
//soke 原来这里是上下对应的，13就是饰品，14就是另外一个饰品，所以15就是时装，16就是另一个时装
//显示的外形对应zObject.h里的OTHERS 和ServerUser里的时装换色来显示外形
//16、17是两个额外包裹，6、7是两个戒指。8、9是两个护腕
const int EquipPack::_poses[] = { 	-1, 	1, 	0, 	3, 	2, 	5, 	8, 	6, 	4, 	10,
									11, 	16,	18,	13,	-1,	15,	9,	7,	-1,	-1,
									12, 	17,	19,	14,	-1,	-1,	-1,	-1,	-1,	-1,
									13, 	31,	32,	33,	34,	35,	36,	37,	38,	39,
									14, 	41,	42,	43,	44, 45, 46, 47,	48, 49,
									15, 	50,	51,	52,	53,	54,	55,	56,	57,	-1,
									16, 	-1,	-1,	-1, -1,	-1,	-1,	-1,	-1,	-1,
									17, 	-1,	-1,	-1, -1,	-1,	-1,	-1,	-1,	-1,
									18, 	-1,	78,	79, 80,	81,	82,	83,	84,	85,
									19, 	86,	87,	-1, -1,	-1,	-1,	-1,	-1,	-1,
									20, 	101, 102, 103, 104, 105, 106, 107, 108, 109,
									21,     110, 111, 112, 113, 114, 115, 116, 117, 118,
									22,     119, 120, 121, 122, 123, 124, 125, 126, 127,
									23,     128, 129, 130, 131, 132, 133, 134, 135, 136};




/**
 * \brief 添加装备
 * \param object 物品对象
 * \param find 是否自动寻找位置
 * \return 添加成功返回true 否则返回false
 */
bool EquipPack::add(zObject *object ,bool find)
{

	//fprintf(stderr,"装备物品EquipPack");
	//isEmpty();
	if (process_extra_add(object)) return Package::add(object,false);

	if (Package::add(object, false))
	{
		//soke  寒冰
		if (owner && object && (object->base->kind == ItemType_Hanbing))
		{
			effecthanbing++;	
			if (1==effecthanbing)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_HANBING_QI, true); 
				Zebra::logger->debug("----------寒冰已装备----------");
			}	
		}

		//soke 翅膀
		if (owner && object && (object->base->kind == ItemType_Xunzhang))
		{
			effectangel++;	
			if (1==effectangel)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_CHIBANG_QI, true); 
				Zebra::logger->debug("----------翅膀已装备----------");
			}	
		}

		if (owner && object && (object->base->kind >= 221 && object->base->kind <= 230))
		{
		   //soke 红色图鉴套装效果
		    // if (owner && object && object->data.handbooklvg >= 81)
			if (owner && object && (601 <= object->data.handbooklvg))
	    	{
		        effecttj7star++;	
		   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活红色图鉴套装效果----------");
	     	   }
		    }
			//soke 橙色图鉴套装效果
		    // if (owner && object && object->data.handbooklvg >= 61)
			if (owner && object && (501 <= object->data.handbooklvg))
		    {
		        effecttj6star++;	
		   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活橙色图鉴套装效果----------");
		       }
	     	}			
			//soke 紫色图鉴套装效果
		    // if (owner && object && object->data.handbooklvg >= 61)
			if (owner && object && (401 <= object->data.handbooklvg))
		    {
		        effecttj5star++;	
		   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj5star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, true); // 更新紫色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活紫色图鉴套装效果----------");
		       }
	     	}			
			//soke 绿色图鉴套装效果
		    // if (owner && object && object->data.handbooklvg >= 61)
			if (owner && object && (301 <= object->data.handbooklvg))
		    {
		        effecttj4star++;	
		   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj5star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, true); // 更新紫色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }		   
		       else if (10<=effecttj4star)
		       {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, true); // 更新绿色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活绿色图鉴套装效果----------");
		       }
	     	}
			//soke 黄色图鉴套装效果
	     	// if (owner && object && object->data.handbooklvg >= 41 )
			if (owner && object && (201 <= object->data.handbooklvg))
	     	{
	     	   effecttj3star++;	
	     	   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj5star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, true); // 更新紫色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }	     	   
	     	   else if (10<=effecttj4star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, true); // 更新绿色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }	     	   
	     	   else if (10<=effecttj3star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, true); // 更新黄色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活黄色图鉴套装效果----------");
	     	   }
	     	}
	     	//soke 蓝色图鉴套装效果
	     	// if (owner && object && object->data.handbooklvg >= 21)
			if (owner && object && (101 <= object->data.handbooklvg))
	     	{
	     	   effecttj2star++;	
	     	   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj5star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, true); // 更新紫色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }	     	   
	     	   else if (10<=effecttj4star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, true); // 更新绿色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }	     	   
	     	   else if (10<=effecttj3star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, true); // 更新黄色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj2star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, true);   // 更新蓝色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活蓝色图鉴套装效果----------");
	     	   }
	     	}
	     	//soke 白色图鉴套装效果
	     	// if (owner && object && object->data.handbooklvg >= 1)
			if (owner && object && (1 <= object->data.handbooklvg))
	     	{
	     	   effecttj1star++;	
	     	   
	     	   if (10<=effecttj7star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, true); // 更新红色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj6star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, true); // 更新橙色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj5star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, true); // 更新紫色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }			   			   	     	   
	     	   else if (10<=effecttj4star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, true); // 更新绿色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }	     	   
	     	   else if (10<=effecttj3star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, true); // 更新黄色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj2star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, true);   // 更新蓝色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     	   }
	     	   else if (10<=effecttj1star)
	     	   {
	     		  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, true); // 更新白色图鉴套装状态
				   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, true); // 更新大波浪
	     		  Zebra::logger->debug("----------激活白色图鉴套装效果----------");
	     	   }
	     	}				
		}		
		//soke 绿装15星套装效果
		if (owner && object && (object->data.bind) && (15 <= object->data.upgrade) && (object->data.kind & 4 || !object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect1star++;	
			if (10<=effect1star)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_EQUIPMENT, true); // 更新绿装15星套装
			//	Zebra::logger->debug("15星绿装x=%d 件",effect1star);
				Zebra::logger->debug("----------激活绿装15星套装效果----------");
			}	
		}
		//soke 紫装套装效果
		if (owner && object && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect2star++;
			if (10<=effect2star)
			{
				owner->showCurrentEffect(Cmd::USTATE_REDHOT, true); // 更新紫装套装状态
			//  Zebra::logger->debug("紫装x=%d,effect2star);
				Zebra::logger->debug("----------激活紫装套装效果----------");
			}
		}

		//soke 永恒套装效果
		if (owner && object && (object->data.needlevel >= 220) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect27star++;	
			//soke 永恒显示的效果 不计入星星套装
		     if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		 	//	Zebra::logger->debug("永恒套装x=%d,effect18star);
			Zebra::logger->debug("----------激活永恒套装效果----------");
		   }
		}
		
		//soke 弑天套装效果
		if (owner && object && (object->data.needlevel >= 215) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect26star++;	
			//soke 弑天显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
			  Zebra::logger->debug("----------激活弑天套装效果----------");
		   }
		}	
		
		//soke 帝王套装效果
		if (owner && object && (object->data.needlevel >= 210) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect25star++;	
			//soke 帝王显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
			  Zebra::logger->debug("----------激活帝王套装效果----------");
		   }
		}
		
		//soke 战神套装效果
		if (owner && object && (object->data.needlevel >= 205) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect24star++;	
			//soke 战神显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
			  Zebra::logger->debug("----------激活战神套装效果----------");
		   }
		}
		
		//soke 圣者套装效果
		if (owner && object && (object->data.needlevel >= 202) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect23star++;	
			//soke 圣者显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
			  Zebra::logger->debug("----------激活圣者套装效果----------");
		   }
		}
		
		//soke 英雄套装效果
		if (owner && object && (object->data.needlevel >= 197) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect22star++;	
			//soke 英雄显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
			  Zebra::logger->debug("----------激活英雄套装效果----------");
		   }
		}
		
		//soke 天尊套装效果
		if (owner && object && (object->data.needlevel >= 190) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect21star++;	
			//soke 天尊显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
			  Zebra::logger->debug("----------激活天尊套装效果----------");
		   }
		}
		
		//soke 传说套装效果
		if (owner && object && (object->data.needlevel >= 185) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect20star++;	
			//soke 传说显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
		   }
		   else if (10<=effect20star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, true); // 更新传说套装状态
			  Zebra::logger->debug("----------激活传说套装效果----------");
		   }
		}
		
		//soke 卓越套装效果
		if (owner && object && (object->data.needlevel >= 180) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect19star++;	
			//soke 卓越套显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
		   }
		   else if (10<=effect20star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, true); // 更新传说套装状态
		   }		   
		   else if (10<=effect19star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, true); // 更新卓越套装状态
			  Zebra::logger->debug("----------激活卓越套装效果----------");
		   }
		}
		
		//soke 轩辕套装效果
		if (owner && object && (object->data.needlevel >= 175) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			effect18star++;	
			//soke 轩辕套显示的效果 不计入星星套装
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
		   }
		   else if (10<=effect20star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, true); // 更新传说套装状态
		   }		   
		   else if (10<=effect19star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, true); // 更新卓越套装状态
		   }		   
		   else if (10<=effect18star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, true); // 更新轩辕套装状态
			  Zebra::logger->debug("----------激活轩辕套装效果----------");
		   }
		}
		
		//soke 炙天套装效果
		if (owner && object && (object->data.needlevel >= 170) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
		   effect17star++;	
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
		   }
		   else if (10<=effect20star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, true); // 更新传说套装状态
		   }		   
		   else if (10<=effect19star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, true); // 更新卓越套装状态
		   }		   
		   else if (10<=effect18star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, true); // 更新轩辕套装状态
		   }
		   else if (10<=effect17star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, true);   // 更新炙天套装状态
			  Zebra::logger->debug("----------激活炙天套装效果----------");
		   }
		}
		
		//soke 乾坤套装效果
		if (owner && object && (object->data.needlevel >= 165) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
		   effect16star++;	
		   
		   if (10<=effect27star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, true); // 更新永恒套装状态
		   }
		   else if (10<=effect26star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, true); // 更新弑天套装状态
		   }		   
		   else if (10<=effect25star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, true); // 更新帝王套装状态
		   }		   
		    else if (10<=effect24star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, true); // 更新战神套装状态
		   }
		   else if (10<=effect23star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, true); // 更新圣者套装状态
		   }
		   else if (10<=effect22star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, true); // 更新英雄套装状态
		   }
		   else if (10<=effect21star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, true); // 更新天尊套装状态
		   }
		   else if (10<=effect20star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, true); // 更新传说套装状态
		   }		   
		   else if (10<=effect19star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, true); // 更新卓越套装状态
		   }		   
		   else if (10<=effect18star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, true); // 更新轩辕套装状态
		   }
		   else if (10<=effect17star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, true);   // 更新炙天套装状态
		   }
		   else if (10<=effect16star)
		   {
			  owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, true); // 更新乾坤套装状态
		      //Zebra::logger->debug("乾坤套装x=%d,effect16star);
			  Zebra::logger->debug("----------激活乾坤套装效果----------");
		   }
		}			
		//soke 星星套装效果 start
		if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			if (owner && object && (15 <= object->data.upgrade))
			{
				effect15star++;
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态

					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活15星套装效果----------");
				}
			}
			if (owner && object && (14 <= object->data.upgrade))
			{
				effect14star++;
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活14星套装效果----------");
				}
			}
			if (owner && object && (13 <= object->data.upgrade))
			{
				effect13star++;		
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活13星套装效果----------");
				}
			}
			if (owner && object && (12 <= object->data.upgrade))
			{
				effect12star++;		
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
				}
				else if (10<=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, true); // 更新12星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活12星套装效果----------");
				}
			}
			if (owner && object && (10 <= object->data.upgrade))
			{
				effect10star++;	
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
				}
				else if (10<=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, true); // 更新12星状态
				}
				else if (10<=effect10star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, true); // 更新10星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活10星套装效果----------");
				}
			}
			if (owner && object && (8 <= object->data.upgrade))
			{
				effect8star++;	
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
				}
				else if (10<=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, true); // 更新12星状态
				}
				else if (10<=effect10star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, true); // 更新10星状态
				}
				else if (10<=effect8star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, true); // 更新8星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活8星套装效果----------");
				}
			}
			if (owner && object && (5 <= object->data.upgrade))
			{
				effect5star++;		
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
				}
				else if (10<=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, true); // 更新12星状态
				}
				else if (10<=effect10star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, true); // 更新10星状态
				}
				else if (10<=effect8star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, true); // 更新8星状态
				}
				else if (10<=effect5star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, true); // 更新5星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活5星套装效果----------");
				}
			}
			if (owner && object && (3 <= object->data.upgrade))
			{
				effect3star++;	
				if (10<=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, true); // 更新15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, true); // 更新15星状态
				}
				else if (10<=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, true); // 更新14星状态
				}
				else if (10<=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, true); // 更新13星状态
				}
				else if (10<=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, true); // 更新12星状态
				}
				else if (10<=effect10star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, true); // 更新10星状态
				}
				else if (10<=effect8star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, true); // 更新8星状态
				}
				else if (10<=effect5star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, true); // 更新5星状态
				}
				else if (10<=effect3star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, true); // 更新3星状态
					Zebra::logger->debug("3x=%d,5x=%d,8x=%d,10x=%d,12x=%d,13x=%d,14x=%d,15x=%d",effect3star,effect5star,effect8star,effect10star,effect12star,effect13star,effect14star,effect15star);
					Zebra::logger->debug("----------激活3星套装效果----------");
				}
			}
		}
		
		// //soke 第二灵魂系统 开始-------------------------------------------------------------
		// if (owner&&object && (object->data.bind && object->data.kind & 16 && 60 == object->data.addpdef && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
		// { //物理防御力增强
		// 	effect21linghun++;	
		// 	if (8<=effect21linghun)
		//     {
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, true);  // 物理防御力增强
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, true); // 物理防御力加成
		//        //Zebra::logger->debug("物理防御力增强=%d,魔法防御力增强=%d,增加物理攻击力=%d,增加魔法攻击力=%d,增加生命值=%d",effect21linghun,effect22linghun,effect23linghun,effect24linghun,effect25linghun);
		// 	   Zebra::logger->debug("----------激活第二灵魂系统-物理防御力增强----------");
		//     }
		// }
		// if (owner&&object && (object->data.bind && object->data.kind & 16 && 60 == object->data.addmdef && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
		// { //魔法防御力增强
		// 	effect22linghun++;	
		//     if (8<=effect22linghun)
		//     {
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, true);  // 魔法防御力增强
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, true); // 魔法防御力加成
		//        //Zebra::logger->debug("物理防御力增强=%d,魔法防御力增强=%d,增加物理攻击力=%d,增加魔法攻击力=%d,增加生命值=%d",effect21linghun,effect22linghun,effect23linghun,effect24linghun,effect25linghun);
		// 	   Zebra::logger->debug("----------激活第二灵魂系统-魔法防御力增强----------");
		//     }
		// }
		// if (owner&&object && (object->data.bind && object->data.kind & 16 && 120 == object->data.addpdam && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
		// { //增加物理攻击力
		// 	effect23linghun++;
		//     if (8<=effect23linghun)
		//     {
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, true);  // 增加物理攻击力
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, true); // 增加物理攻击力加成
		//        //Zebra::logger->debug("物理防御力增强=%d,魔法防御力增强=%d,增加物理攻击力=%d,增加魔法攻击力=%d,增加生命值=%d",effect21linghun,effect22linghun,effect23linghun,effect24linghun,effect25linghun;
		// 	   Zebra::logger->debug("----------激活第二灵魂系统-增加物理攻击力----------");
		//     }
		// }
		// if (owner&&object && (object->data.bind && object->data.kind & 16 && 120 == object->data.addmdam && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
		// { //增加魔法攻击力
		// 	effect24linghun++;		
		//     if (8<=effect24linghun)
		//     {
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, true);  // 增加魔法攻击力
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, true); // 增加魔法攻击力加成
		//        //Zebra::logger->debug("物理防御力增强=%d,魔法防御力增强=%d,增加物理攻击力=%d,增加魔法攻击力=%d,增加生命值=%d",effect21linghun,effect22linghun,effect23linghun,effect24linghun,effect25linghun);
		// 	   Zebra::logger->debug("----------激活第二灵魂系统-增加魔法攻击力----------");
		//     }
		// }
		// if (owner&&object && (object->data.bind && object->data.kind & 16 && 100 == object->data.addhp && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
		// { //增加生命值
		// 	effect25linghun++;	
		//     if (8<=effect25linghun)
		//     {
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, true);  // 增加生命值
		// 	   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, true); // 增加生命值加成
		//        //Zebra::logger->debug("物理防御力增强=%d,魔法防御力增强=%d,增加物理攻击力=%d,增加魔法攻击力=%d,增加生命值=%d",effect21linghun,effect22linghun,effect23linghun,effect24linghun,effect25linghun);
		// 	   Zebra::logger->debug("----------激活第二灵魂系统-增加生命值----------");
		//     }
		// }
		// //soke 第二灵魂系统 结束-------------------------------------------------------------
		
		//soke 全套15星斗魂套装 （装备必须控制激活最大就是10件）
	    if(owner && object && object->data.bind && (object->data.kind & 16) && ( (object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155) )
	    {
			if (owner&&object &&(15 <= object->data.douhun))
			{
				effect15douhun++;
               if (10<=effect15douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, true); // 更新全套15星斗魂套装
				//Zebra::logger->debug("3x斗魂=%d","6x斗魂=%d","9x斗魂=%d","12x斗魂=%d","15x斗魂=%d",effect3douhun,effect6douhun,effect9douhun,effect12douhun,effect15douhun);
				Zebra::logger->debug("----------激活15星斗魂套装效果----------");
			   } 
    		}      
		    //soke 全套12星斗魂套装 （装备必须控制激活最大就是10件）
      		if (owner&&object &&(12 <= object->data.douhun))
			{
				effect12douhun++;
               if (10<=effect15douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, true); // 更新全套15星斗魂套装
			   }
			   else if (10<=effect12douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, true); // 更新全套12星斗魂套装
				//Zebra::logger->debug("3x斗魂=%d","6x斗魂=%d","9x斗魂=%d","12x斗魂=%d","15x斗魂=%d",effect3douhun,effect6douhun,effect9douhun,effect12douhun,effect15douhun);
				Zebra::logger->debug("----------激活12星斗魂套装效果----------");
			   } 
    		}       		
		    //soke 全套9星斗魂套装 （装备必须控制激活最大就是10件）
			if (owner&&object &&(9 <= object->data.douhun))
			{
				effect9douhun++;
               if (10<=effect15douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, true); // 更新全套15星斗魂套装
			   }
			   else if (10<=effect12douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, true); // 更新全套12星斗魂套装
			   }
			   else if (10<=effect9douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, true); // 更新全套9星斗魂套装
				//Zebra::logger->debug("3x斗魂=%d","6x斗魂=%d","9x斗魂=%d","12x斗魂=%d","15x斗魂=%d",effect3douhun,effect6douhun,effect9douhun,effect12douhun,effect15douhun);
				Zebra::logger->debug("----------激活9星斗魂套装效果----------");
			   } 
    		}
		    //soke 全套6星斗魂套装 （装备必须控制激活最大就是10件）
			if (owner&&object &&(6 <= object->data.douhun))
			{
				effect6douhun++;
               if (10<=effect15douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, true); // 更新全套15星斗魂套装
			   }
			   else if (10<=effect12douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, true); // 更新全套12星斗魂套装
			   }
			   else if (10<=effect9douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, true); // 更新全套9星斗魂套装
			   }
			   else if (10<=effect6douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, true); // 更新全套6星斗魂套装
				//Zebra::logger->debug("3x斗魂=%d","6x斗魂=%d","9x斗魂=%d","12x斗魂=%d","15x斗魂=%d",effect3douhun,effect6douhun,effect9douhun,effect12douhun,effect15douhun);
				Zebra::logger->debug("----------激活6星斗魂套装效果----------");
			   } 
    		}
		    //soke 全套3星斗魂套装 （装备必须控制激活最大就是10件）
			if (owner&&object &&(3 <= object->data.douhun))
			{
				effect3douhun++;
               if (10<=effect15douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, true); // 更新全套15星斗魂套装
			   }
			   else if (10<=effect12douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, true); // 更新全套12星斗魂套装
			   }
			   else if (10<=effect9douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, true); // 更新全套9星斗魂套装
			   }
			   else if (10<=effect6douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, true); // 更新全套6星斗魂套装
			   }
			   else if (10<=effect3douhun)
			   {
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, true); // 更新全套3星斗魂套装
				//Zebra::logger->debug("3x斗魂=%d","6x斗魂=%d","9x斗魂=%d","12x斗魂=%d","15x斗魂=%d",effect3douhun,effect6douhun,effect9douhun,effect12douhun,effect15douhun);
				Zebra::logger->debug("----------激活3星斗魂套装效果----------");
			   } 
    		}
        }

		//soke 龙星套装效果 start
		if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			if (owner && object && (15 <= object->data.drastar))
			{
				effect15longx++;
				if (10<=effect15longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, true); // 更新15x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				
					Zebra::logger->debug(",5x龙星=%d,8x龙星=%d,11x龙星=%d,13x龙星=%d,15x龙星=%d",effect5longx,effect8longx,effect11longx,effect13longx,effect15longx);
					Zebra::logger->debug("----------激活15x龙星套装效果----------");
					
				}
			}
			if (owner && object && (13 <= object->data.drastar))
			{
				effect13longx++;		
				if (10<=effect15longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, true); // 更新15x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect13longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, true); // 更新13x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					Zebra::logger->debug(",5x龙星=%d,8x龙星=%d,11x龙星=%d,13x龙星=%d,15x龙星=%d",effect5longx,effect8longx,effect11longx,effect13longx,effect15longx);
					Zebra::logger->debug("----------激活13x龙星套装效果----------");
					
				}
			}
			if (owner && object && (11 <= object->data.drastar))
			{
				effect11longx++;	
				if (10<=effect15longx)
				{
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, true); // 更新15x龙星状态
					
				}
				else if (10<=effect13longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, true); // 更新13x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect11longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, true); // 更新11x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					Zebra::logger->debug(",5x龙星=%d,8x龙星=%d,11x=%d,13x龙星=%d,15x龙星=%d",effect5longx,effect8longx,effect11longx,effect13longx,effect15longx);
					Zebra::logger->debug("----------激活11x龙星套装效果----------");
				}
			}
			if (owner && object && (8 <= object->data.drastar))
			{
				effect8longx++;	
				if (10<=effect15longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, true); // 更新15x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect13longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, true); // 更新13x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect11longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, true); // 更新11x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect8longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, true); // 更新8x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					Zebra::logger->debug(",5x龙星=%d,8x龙星=%d,11x龙星=%d,13x龙星=%d,15x龙星=%d",effect5longx,effect8longx,effect11longx,effect13longx,effect15longx);
					Zebra::logger->debug("----------激活8x龙星套装效果----------");
				}
			}
			if (owner && object && (5 <= object->data.drastar))
			{
				effect5longx++;		
				if (10<=effect15longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, true); // 更新15x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect13longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, true); // 更新13x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect11longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, true); // 更新11x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
				}
				else if (10<=effect8longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, true); // 更新8x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					
				}
				else if (10<=effect5longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING , true); // 更新5x龙星状态
					if(owner->issetUState(Cmd::USTATE_ULTRA_FIF_XING))
					{
						owner->clearUState(Cmd::USTATE_ULTRA_FIF_XING);
					}
					Zebra::logger->debug(",5x龙星=%d,8x龙星=%d,11x龙星=%d,13x龙星=%d,15x龙星=%d",effect5longx,effect8longx,effect11longx,effect13longx,effect15longx);
					Zebra::logger->debug("----------激活5x龙星套装效果----------");
				}
			}
		}

		//soke 补天套装效果 start
		if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			// if (owner && object && (5 <= object->data.butian))
			// {
			// 	effect5butian++;
			// 	if (10<=effect5butian)
			// 	{
			// 		owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, true); // 更新补天状态
			// 		Zebra::logger->debug(",1x补天=%d,2x补天=%d,3x补天=%d,4x补天=%d,5x补天=%d",effect1butian,effect2butian,effect3butian,effect4butian,effect1butian);
			// 		Zebra::logger->debug("----------激活5x补天套装效果----------");
			// 	}
			// }
			if (owner && object && (4 <= object->data.butian))
			{
				effect4butian++;		
				if (10<=effect5butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, true); // 更新补天状态
				}
				else if (10<=effect4butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, true); // 更新补天状态
					Zebra::logger->debug(",1x补天=%d,2x补天=%d,3x补天=%d,4x补天=%d,5x补天=%d",effect1butian,effect2butian,effect3butian,effect4butian,effect1butian);
					Zebra::logger->debug("----------激活4x补天套装效果----------");
				}
			}
			if (owner && object && (3 <= object->data.butian))
			{
				effect3butian++;	
				if (10<=effect5butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, true); // 更新补天状态
				}
				else if (10<=effect4butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, true); // 更新补天状态
				}
				else if (10<=effect3butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, true); // 更新补天状态
					Zebra::logger->debug(",1x补天=%d,2x补天=%d,3x=%d,4x补天=%d,5x补天=%d",effect1butian,effect2butian,effect3butian,effect4butian,effect1butian);
					Zebra::logger->debug("----------激活3x补天套装效果----------");
				}
			}
			if (owner && object && (2 <= object->data.butian))
			{
				effect2butian++;	
				if (10<=effect5butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, true); // 更新补天状态
				}
				else if (10<=effect4butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, true); // 更新补天状态
				}
				else if (10<=effect3butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, true); // 更新补天状态
				}
				else if (10<=effect2butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, true); // 更新补天状态
					Zebra::logger->debug(",1x补天=%d,2x补天=%d,3x补天=%d,4x补天=%d,5x补天=%d",effect1butian,effect2butian,effect3butian,effect4butian,effect1butian);
					Zebra::logger->debug("----------激活2x补天套装效果----------");
				}
			}
			if (owner && object && (1 <= object->data.butian))
			{
				effect1butian++;		
				if (10<=effect5butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, true); // 更新补天状态
				}
				else if (10<=effect4butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, true); // 更新补天状态
				}
				else if (10<=effect3butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, true); // 更新补天状态
				}
				else if (10<=effect2butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, true); // 更新补天状态
				}
				else if (10<=effect1butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI , true); // 更新补天状态
					Zebra::logger->debug(",1x补天=%d,2x补天=%d,3x补天=%d,4x补天=%d,5x补天=%d",effect1butian,effect2butian,effect3butian,effect4butian,effect1butian);
					Zebra::logger->debug("----------激活1x补天套装效果----------");
				}
			}
		}

		//soke 装备栏套装效果 start
		if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			if (owner && object && (6 <= object->data.retain2))
			{
				effect6zhuangbei++;
				if (10<=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, true); // 更新装备栏状态
					Zebra::logger->debug(",2x装备栏=%d,3x装备栏=%d,4x装备栏=%d,5x装备栏=%d,6x装备栏=%d",effect2zhuangbei,effect3zhuangbei,effect4zhuangbei,effect5zhuangbei,effect6zhuangbei);
					Zebra::logger->debug("----------激活6x装备栏套装效果----------");
				}				
			}
			if (owner && object && (5 <= object->data.retain2))
			{
				effect5zhuangbei++;
				if (10<=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, true); // 更新装备栏状态
				}				
				if (10<=effect5zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, true); // 更新装备栏状态
					Zebra::logger->debug(",2x装备栏=%d,3x装备栏=%d,4x装备栏=%d,5x装备栏=%d,6x装备栏=%d",effect2zhuangbei,effect3zhuangbei,effect4zhuangbei,effect5zhuangbei,effect6zhuangbei);
					Zebra::logger->debug("----------激活5x装备栏套装效果----------");
				}
			}
			if (owner && object && (4 <= object->data.retain2))
			{
				effect4zhuangbei++;
				if (10<=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, true); // 更新装备栏状态
				}				
				if (10<=effect5zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, true); // 更新装备栏状态
				}
				else if (10<=effect4zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, true); // 更新装备栏状态
					Zebra::logger->debug(",2x装备栏=%d,3x装备栏=%d,4x装备栏=%d,5x装备栏=%d,6x装备栏=%d",effect2zhuangbei,effect3zhuangbei,effect4zhuangbei,effect5zhuangbei,effect6zhuangbei);
					Zebra::logger->debug("----------激活4x装备栏套装效果----------");
				}
			}
			if (owner && object && (3 <= object->data.retain2))
			{
				effect3zhuangbei++;
				if (10<=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, true); // 更新装备栏状态
				}				
				if (10<=effect5zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, true); // 更新装备栏状态
				}
				else if (10<=effect4zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, true); // 更新装备栏状态
				}
				else if (10<=effect3zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, true); // 更新装备栏状态
					Zebra::logger->debug(",2x装备栏=%d,3x装备栏=%d,4x装备栏=%d,5x装备栏=%d,6x装备栏=%d",effect2zhuangbei,effect3zhuangbei,effect4zhuangbei,effect5zhuangbei,effect6zhuangbei);
					Zebra::logger->debug("----------激活3x装备栏套装效果----------");
				}
			}
			if (owner && object && (2 <= object->data.retain2))
			{
				effect2zhuangbei++;
				if (10<=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, true); // 更新装备栏状态
				}				
				if (10<=effect5zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, true); // 更新装备栏状态
				}
				else if (10<=effect4zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, true); // 更新装备栏状态
				}
				else if (10<=effect3zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, true); // 更新装备栏状态
				}
				else if (10<=effect2zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, true); // 更新装备栏状态
					Zebra::logger->debug(",2x装备栏=%d,3x装备栏=%d,4x装备栏=%d,5x装备栏=%d,6x装备栏=%d",effect2zhuangbei,effect3zhuangbei,effect4zhuangbei,effect5zhuangbei,effect6zhuangbei);
					Zebra::logger->debug("----------激活2x装备栏套装效果----------");
				}
			}
		}
		
		//soke 神器套装效果 start
		if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
			if (owner && object && (7 <= object->data.retain3))
			{
				effect7shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活7x神器套装效果----------");
				}			
			}
			if (owner && object && (6 <= object->data.retain3))
			{
				effect6shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活6x神器套装效果----------");
				}				
			}
			if (owner && object && (5 <= object->data.retain3))
			{
				effect5shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
				}				
				if (10<=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, true); // 更新完美神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活5x神器套装效果----------");
				}
			}
			if (owner && object && (4 <= object->data.retain3))
			{
				effect4shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
				}				
				if (10<=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, true); // 更新完美神器状态
				}
				else if (10<=effect4shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, true); // 更新无暇神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活4x神器套装效果----------");
				}
			}
			if (owner && object && (3 <= object->data.retain3))
			{
				effect3shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
				}				
				if (10<=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, true); // 更新完美神器状态
				}
				else if (10<=effect4shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, true); // 更新无暇神器状态
				}
				else if (10<=effect3shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, true); // 更新精致神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活3x神器套装效果----------");
				}
			}
			if (owner && object && (2 <= object->data.retain3))
			{
				effect2shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
				}				
				if (10<=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, true); // 更新完美神器状态
				}
				else if (10<=effect4shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, true); // 更新无暇神器状态
				}
				else if (10<=effect3shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, true); // 更新精致神器状态
				}
				else if (10<=effect2shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, true); // 更新强化神器状态
					Zebra::logger->debug(",1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活2x神器套装效果----------");
				}
			}
			if (owner && object && (1 <= object->data.retain3))
			{
				effect1shenqi++;
				if (10<=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, true); // 更新卓越神器状态
				}
				if (10<=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, true); // 更新华丽神器状态
				}				
				if (10<=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, true); // 更新完美神器状态
				}
				else if (10<=effect4shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, true); // 更新无暇神器状态
				}
				else if (10<=effect3shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, true); // 更新精致神器状态
				}
				else if (10<=effect2shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, true); // 更新强化神器状态
				}
				else if (10<=effect1shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI , true); // 更新实用神器状态
					Zebra::logger->debug("1x神器=%d,2x神器=%d,3x神器=%d,4x神器=%d,5x神器=%d,6x神器=%d,7x神器=%d",effect1shenqi,effect2shenqi,effect3shenqi,effect4shenqi,effect5shenqi,effect6shenqi,effect7shenqi);
					Zebra::logger->debug("----------激活1x神器套装效果----------");
				}
			}
		}
		
		//soke 神龙套装效果 start////////////////////////////////////////////////////////////////////////////////////////////////////////
	    if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
            if (owner && object && (6 <= object->data.longwanglv))
			{
				effect6shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
                    Zebra::logger->debug("----------激活神巨龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
            if (owner && object && (5 <= object->data.longwanglv))
			{
				effect5shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
				}	
				if (10<=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, true); // 更新神苍龙套装状态
                    Zebra::logger->debug("----------激活神苍龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
            if (owner && object && (4 <= object->data.longwanglv))
			{
				effect4shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
				}	
				if (10<=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, true); // 更新神苍龙套装状态
				}
				else if (10<=effect4shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, true); // 更新巨龙套装状态
                    Zebra::logger->debug("----------激活巨龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
            if (owner && object && (3 <= object->data.longwanglv))
			{
				effect3shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
				}	
				if (10<=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, true); // 更新神苍龙套装状态
				}
				else if (10<=effect4shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, true); // 更新巨龙套装状态
				}
				else if (10<=effect3shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, true); // 更新苍龙套装状态
                    Zebra::logger->debug("----------激活苍龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
            if (owner && object && (2 <= object->data.longwanglv))
			{
				effect2shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
				}	
				if (10<=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, true); // 更新神苍龙套装状态
				}
				else if (10<=effect4shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, true); // 更新巨龙套装状态
				}
				else if (10<=effect3shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, true); // 更新苍龙套装状态
				}
				else if (10<=effect2shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, true); // 更新金龙套装状态
                    Zebra::logger->debug("----------激活金龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
			if (owner && object && (1 <= object->data.longwanglv))
			{
				effect1shenlong++;	
                if (10<=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, true); // 更新神巨龙套装状态
				}	
				if (10<=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, true); // 更新神苍龙套装状态
				}
				else if (10<=effect4shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, true); // 更新巨龙套装状态
				}
				else if (10<=effect3shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, true); // 更新苍龙套装状态
				}
				else if (10<=effect2shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, true); // 更新金龙套装状态
				}
				else if (10<=effect1shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1 , true); // 更新小龙套装状态
                    Zebra::logger->debug("----------激活小龙套装效果(10件)----------");
					Zebra::logger->debug("1x小龙=%d件,2x金龙=%d件,3x苍龙=%d件,4x巨龙=%d件,5x神苍龙=%d件,6x神巨龙=%d件",effect1shenlong,effect2shenlong,effect3shenlong,effect4shenlong,effect5shenlong,effect6shenlong);
		
				}
			}
		}

		//soke 逆天进阶套装效果 start////////////////////////////////////////////////////////////////////////////////////////////////////////
	    if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
            if (owner && object && (100 <= object->data.nitianlv))
			{
				effect5nitian++;	
				if (10<=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, true); // 更新100阶逆天套装状态
                    Zebra::logger->debug("----------激活100阶逆天装备套装效果(10件)----------");
					Zebra::logger->debug("15阶逆天=%d件,30阶逆天=%d件,50阶逆天=%d件,80阶逆天=%d件,100阶逆天=%d件",effect1nitian,effect2nitian,effect3nitian,effect4nitian,effect5nitian);		
				}
			}
            if (owner && object && (80 <= object->data.nitianlv))
			{
				effect4nitian++;	
				if (10<=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, true); // 更新100阶逆天套装状态
				}
				else if (10<=effect4nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, true);  //更新80阶逆天套装状态
                    Zebra::logger->debug("----------激活80阶逆天装备套装效果(10件)----------");
					Zebra::logger->debug("15阶逆天=%d件,30阶逆天=%d件,50阶逆天=%d件,80阶逆天=%d件,100阶逆天=%d件",effect1nitian,effect2nitian,effect3nitian,effect4nitian,effect5nitian);
		
				}
			}
            if (owner && object && (50 <= object->data.nitianlv))
			{
				effect3nitian++;	
				if (10<=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, true); // 更新100阶逆天套装状态
				}
				else if (10<=effect4nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, true); // 更新80阶逆天套装状态
				}
				else if (10<=effect3nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, true); // 更新50阶逆天套装状态
                    Zebra::logger->debug("----------激活50阶逆天装备套装效果(10件)----------");
					Zebra::logger->debug("15阶逆天=%d件,30阶逆天=%d件,50阶逆天=%d件,80阶逆天=%d件,100阶逆天=%d件",effect1nitian,effect2nitian,effect3nitian,effect4nitian,effect5nitian);		
				}
			}
            if (owner && object && (30 <= object->data.nitianlv))
			{
				effect2nitian++;	
				if (10<=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, true); // 更新100阶逆天套装状态
				}
				else if (10<=effect4nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, true); // 更新80阶逆天套装状态
				}
				else if (10<=effect3nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, true); // 更新50阶逆天套装状态
				}
				else if (10<=effect2nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, true); // 更新30阶逆天套装状态
                    Zebra::logger->debug("----------激活30阶逆天装备套装效果(10件)----------");
					Zebra::logger->debug("15阶逆天=%d件,30阶逆天=%d件,50阶逆天=%d件,80阶逆天=%d件,100阶逆天=%d件",effect1nitian,effect2nitian,effect3nitian,effect4nitian,effect5nitian);
		
				}
			}
			if (owner && object && (15 <= object->data.nitianlv))
			{
				effect1nitian++;	
				if (10<=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, true); // 更新100阶逆天套装状态
				}
				else if (10<=effect4nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, true); // 更新80阶逆天套装状态
				}
				else if (10<=effect3nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, true); // 更新50阶逆天套装状态
				}
				else if (10<=effect2nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, true); // 更新30阶逆天套装状态
				}
				else if (10<=effect1nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1 , true); // 更新15阶逆天套装状态
                    Zebra::logger->debug("----------激活15阶逆天装备套装效果(10件)----------");
					Zebra::logger->debug("15阶逆天=%d件,30阶逆天=%d件,50阶逆天=%d件,80阶逆天=%d件,100阶逆天=%d件",effect1nitian,effect2nitian,effect3nitian,effect4nitian,effect5nitian);
		
				}
			}
		}

        	//soke 神魂宝石套装效果 start////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
            if (owner && object && (100 <= object->data.douzhansflv))
			{
				effect4douzhansf++;	
				if (10<=effect4douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, true);  //更新100阶神魂宝石套装状态
                    Zebra::logger->debug("----------激活100阶神魂宝石装备套装效果(10件)----------");
					Zebra::logger->debug("25阶神魂宝石=%d件,50阶神魂宝石=%d件,75阶神魂宝石=%d件,100阶神魂宝石=%d件",effect1douzhansf,effect2douzhansf,effect3douzhansf,effect4douzhansf);
		
				}
			}
            if (owner && object && (75 <= object->data.douzhansflv))
			{
				effect3douzhansf++;	
			    if (10<=effect4douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, true); // 更新100阶神魂宝石套装状态
				}
				else if (10<=effect3douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, true); // 更新75阶神魂宝石套装状态
                    Zebra::logger->debug("----------激活75阶神魂宝石装备套装效果(10件)----------");
					Zebra::logger->debug("25阶神魂宝石=%d件,50阶神魂宝石=%d件,75阶神魂宝石=%d件,100阶神魂宝石=%d件",effect1douzhansf,effect2douzhansf,effect3douzhansf,effect4douzhansf);
				}
			}
            if (owner && object && (50 <= object->data.douzhansflv))
			{
				effect2douzhansf++;	
				if (10<=effect4douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, true); // 更新100阶神魂宝石套装状态
				}
				else if (10<=effect3douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, true); // 更新75阶神魂宝石套装状态
				}
				else if (10<=effect2douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, true); // 更新50阶神魂宝石套装状态
                    Zebra::logger->debug("----------激活50阶神魂宝石装备套装效果(10件)----------");
					Zebra::logger->debug("25阶神魂宝石=%d件,50阶神魂宝石=%d件,75阶神魂宝石=%d件,100阶神魂宝石=%d件",effect1douzhansf,effect2douzhansf,effect3douzhansf,effect4douzhansf);
		
				}
			}
			if (owner && object && (20 <= object->data.douzhansflv))
			{
				effect1douzhansf++;	
			    if (10<=effect4douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, true); // 更新100阶神魂宝石套装状态
				}
				else if (10<=effect3douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, true); // 更新75阶神魂宝石套装状态
				}
				else if (10<=effect2douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, true); // 更新50阶神魂宝石套装状态
				}
				else if (10<=effect1douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV1 , true); // 更新25阶神魂宝石套装状态
                    Zebra::logger->debug("----------激活25阶神魂宝石装备套装效果(10件)----------");
					Zebra::logger->debug("25阶神魂宝石=%d件,50阶神魂宝石=%d件,75阶神魂宝石=%d件,100阶神魂宝石=%d件",effect1douzhansf,effect2douzhansf,effect3douzhansf,effect4douzhansf);
		
				}
			}
		}

	    //soke 暗影宝石套装效果 start////////////////////////////////////////////////////////////////////////////////////////////////////////
	    if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
            if (owner && object && (100 <= object->data.aystonelv))
			{
				effect5aystone++;	
				if (10<=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, true); // 更新100阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活100阶暗影宝石装备套装效果(10件)----------");
					Zebra::logger->debug("15阶暗影宝石=%d件,30阶暗影宝石=%d件,50阶暗影宝石=%d件,80阶暗影宝石=%d件,100阶暗影宝石=%d件",effect1aystone,effect2aystone,effect3aystone,effect4aystone,effect5aystone);		
				}
			}
            if (owner && object && (80 <= object->data.aystonelv))
			{
				effect4aystone++;	
				if (10<=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, true);  //更新80阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活80阶暗影宝石装备套装效果(10件)----------");
					Zebra::logger->debug("15阶暗影宝石=%d件,30阶暗影宝石=%d件,50阶暗影宝石=%d件,80阶暗影宝石=%d件,100阶暗影宝石=%d件",effect1aystone,effect2aystone,effect3aystone,effect4aystone,effect5aystone);
		
				}
			}
            if (owner && object && (60 <= object->data.aystonelv))
			{
				effect3aystone++;	
				if (10<=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, true); // 更新60阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活60阶暗影宝石装备套装效果(10件)----------");
					Zebra::logger->debug("15阶暗影宝石=%d件,30阶暗影宝石=%d件,50阶暗影宝石=%d件,80阶暗影宝石=%d件,100阶暗影宝石=%d件",effect1aystone,effect2aystone,effect3aystone,effect4aystone,effect5aystone);		
				}
			}
            if (owner && object && (40 <= object->data.aystonelv))
			{
				effect2aystone++;	
				if (10<=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, true); // 更新60阶暗影宝石套装状态
				}
				else if (10<=effect2aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, true); // 更新40阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活40阶暗影宝石装备套装效果(10件)----------");
					Zebra::logger->debug("15阶暗影宝石=%d件,30阶暗影宝石=%d件,50阶暗影宝石=%d件,80阶暗影宝石=%d件,100阶暗影宝石=%d件",effect1aystone,effect2aystone,effect3aystone,effect4aystone,effect5aystone);
		
				}
			}
			if (owner && object && (20 <= object->data.aystonelv))
			{
				effect1aystone++;	
				if (10<=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, true); // 更新60阶暗影宝石套装状态
				}
				else if (10<=effect2aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, true); // 更新40阶暗影宝石套装状态
				}
				else if (10<=effect1aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1 , true); // 更新20阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活20阶暗影宝石装备套装效果(10件)----------");
					Zebra::logger->debug("15阶暗影宝石=%d件,30阶暗影宝石=%d件,50阶暗影宝石=%d件,80阶暗影宝石=%d件,100阶暗影宝石=%d件",effect1aystone,effect2aystone,effect3aystone,effect4aystone,effect5aystone);
		
				}
			}
		}

		//soke 龙凤吟套装效果 start////////////////////////////////////////////////////////////////////////////////////////////////////////
	    if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
		{
            if (owner && object && (16 <= object->data.retain24))
			{
				effect5lfy++;	
				if (10<=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, true); // 更新100阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活龙凤吟16级装备套装效果(10件)----------");
				}
			}
            if (owner && object && (14 <= object->data.retain24))
			{
				effect4lfy++;	
				if (10<=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, true);  //更新80阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活龙凤吟14级装备套装效果(10件)----------");
					
				}
			}
            if (owner && object && (12 <= object->data.retain24))
			{
				effect3lfy++;	
				if (10<=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, true); // 更新60阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活龙凤吟12级装备套装效果(10件)----------");
				}
			}
            if (owner && object && (10 <= object->data.retain24))
			{
				effect2lfy++;	
				if (10<=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, true); // 更新60阶暗影宝石套装状态
				}
				else if (10<=effect2lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, true); // 更新40阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活龙凤吟10级装备套装效果(10件)----------");
					
				}
			}
			if (owner && object && (8 <= object->data.retain24))
			{
				effect1lfy++;	
				if (10<=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, true); // 更新100阶暗影宝石套装状态
				}
				else if (10<=effect4lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, true); // 更新80阶暗影宝石套装状态
				}
				else if (10<=effect3lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, true); // 更新60阶暗影宝石套装状态
				}
				else if (10<=effect2lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, true); // 更新40阶暗影宝石套装状态
				}
				else if (10<=effect1lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1 , true); // 更新20阶暗影宝石套装状态
                    Zebra::logger->debug("----------激活龙凤吟8级装备套装效果(10件)----------");
					
				}
			}
		}

		calcAll();
		needRecalc=true;
		return true;
	}
	return false;
}

/**
 * \brief 删除装备
 * \param object 物品对象
 */
bool EquipPack::remove(zObject *object)
{
	if (process_extra_remove(object)) return Package::remove(object);

	bool ret = Package::remove(object);
	calcAll();

	//soke 寒冰
	if (owner && object && (object->base->kind == ItemType_Hanbing ))
	{
		if (effecthanbing>0)
		{
			effecthanbing--;	
			if (0>=effecthanbing)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_HANBING_QI, false);      // 清除寒冰状态
			}
		}
	}

	//soke 翅膀
	if (owner && object && (object->base->kind == ItemType_Xunzhang ))
	{
		if (effectangel>0)
		{
			effectangel--;	
			if (0>=effectangel)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_CHIBANG_QI, false);      // 清除翅膀状态
			}
		}
	}

	//soke 神圣绿装15星套装效果
	if (owner && object && (object->data.bind) && (15 <= object->data.upgrade) && (object->data.kind & 4 || !object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect1star>0)
		{
			effect1star--;	
		 	if (9>=effect1star)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_EQUIPMENT, false);      // 清除绿装15星套装状态
				owner->showCurrentEffect(Cmd::USTATE_REDHOT, false);               // 清除紫装套装状态
			}
		}
	}
	//soke 紫装套装效果
	if (owner && object && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect2star>0)
		{
			effect2star--;	
			if (9>=effect2star)
			{
				owner->showCurrentEffect(Cmd::USTATE_ULTRA_EQUIPMENT, false);      // 清除绿装15星套装状态
				owner->showCurrentEffect(Cmd::USTATE_REDHOT, false);               // 清除紫装套装状态
			}
		}
	}
	
  //soke 永恒装套装效果
	if (owner && object && (object->data.needlevel >= 220) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect27star>0)
		{
			effect27star--;	
			if (9>=effect27star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
  //soke 弑天装套装效果
	if (owner && object && (object->data.needlevel >= 215) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect26star>0)
		{
			effect26star--;	
			if (9>=effect26star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}	
  //soke 帝王装套装效果
	if (owner && object && (object->data.needlevel >= 210) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect25star>0)
		{
			effect25star--;	
			if (9>=effect25star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}

  //soke 战神装套装效果
	if (owner && object && (object->data.needlevel >= 205) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect24star>0)
		{
			effect24star--;	
			if (9>=effect24star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}

    //soke 圣者装套装效果
	if (owner && object && (object->data.needlevel >= 202) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect23star>0)
		{
			effect23star--;	
			if (9>=effect23star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
	
	//soke 英雄装套装效果
	if (owner && object && (object->data.needlevel >= 197) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect22star>0)
		{
			effect22star--;	
			if (9>=effect22star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}

	//soke 天尊装套装效果
	if (owner && object && (object->data.needlevel >= 190) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect21star>0)
		{
			effect21star--;	
			if (9>=effect21star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
	//soke 传说装套装效果
	if (owner && object && (object->data.needlevel >= 185) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect20star>0)
		{
			effect20star--;	
			if (9>=effect20star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}	
	//soke 卓越装套装效果
	if (owner && object && (object->data.needlevel >= 180) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect19star>0)
		{
			effect19star--;	
			if (9>=effect19star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
	//soke 轩辕装套装效果
	if (owner && object && (object->data.needlevel >= 175) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect18star>0)
		{
			effect18star--;	
			if (9>=effect18star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
	//soke 炙天装备显示的效果 不计入星星套装
	if (owner && object && (object->data.needlevel >= 170) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect17star>0)
		{
			effect17star--;	
			if (9>=effect17star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
			   
		    }
		}
	}
	//soke 乾坤装备显示的效果 不计入星星套装
	if (owner && object && (object->data.needlevel >= 165) && (object->data.bind) && (object->data.kind & 4 || object->data.kind & 16)  && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (effect16star>0)
		{
			effect16star--;	
			if (9>=effect16star)
		    {
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YONGHENG, false);    // 清除永恒套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHITIAN, false);     // 清除弑天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DIWANG, false);      // 清除帝王套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHANSHEN, false);    // 清除战神套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_SHENGZHE, false);    // 清除圣者套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_YINGXIONG, false);   // 清除英雄套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TIANZUN, false);     // 清除天尊套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_CHUANSHUO, false);   // 清除传说套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHUOYUE, false);     // 清除卓越套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_XUANYUAN, false);    // 清除轩辕套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_ZHITIAN, false);     // 清除炙天套装状态
			   owner->showCurrentEffect(Cmd::USTATE_USER_TZ_QIANKUN, false);     // 清除乾坤套装状态
		    }
		}
	}
    //soke 红色图鉴套装效果
	if (owner && object && (object->base->kind >= 221 && object->base->kind <= 230))
	{
       // if (owner && object && (object->data.handbooklvg <= 81))
	   if (owner && object && (601 <= object->data.handbooklvg))
		{
		   if (effecttj7star>0)
		    {
			    effecttj7star--;	
			    if (9>=effecttj7star)
		      {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		       }  
		    }
	    }	
         //soke 橙色图鉴套装效果
        // if (owner && object && (object->data.handbooklvg <= 61))
		if (owner && object && (501 <= object->data.handbooklvg))
		{
		    if (effecttj6star>0)
		    {
			     effecttj6star--;	
			    if (9>=effecttj6star)
		        {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		        }  
		    }
	    }
         //soke 紫色图鉴套装效果
        // if (owner && object && (object->data.handbooklvg <= 61))
		if (owner && object && (401 <= object->data.handbooklvg))
		{
		    if (effecttj5star>0)
		    {
			     effecttj5star--;	
			    if (9>=effecttj5star)
		        {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		        }  
		    }
	    }				
         //soke 绿色图鉴套装效果
        // if (owner && object && (object->data.handbooklvg <= 61))
		if (owner && object && (301 <= object->data.handbooklvg))
		{
		    if (effecttj4star>0)
		    {
			     effecttj4star--;	
			    if (9>=effecttj4star)
		        {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		        }  
		    }
	    }	
	   //soke 黄色图鉴套装效果
	   // if (owner && object && (object->data.handbooklvg <= 41))
	   if (owner && object && (201 <= object->data.handbooklvg))
		{
		   if (effecttj3star>0)
		    {
			     effecttj3star--;	
			    if (9>=effecttj3star)
		        {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		       }  
		    }
	    }		
        //soke 蓝色图鉴套装效果
	    // if (owner && object && (object->data.handbooklvg <= 21))
		if (owner && object && (101 <= object->data.handbooklvg))
		{
		    if (effecttj2star>0)
		    {
			    effecttj2star--;	

			    if (9>=effecttj2star)
		        {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		        }  
		    }
	    }	
       //soke 白色图鉴套装效果
	    // if (owner && object && (object->data.handbooklvg <= 1))
		if (owner && object && (1 <= object->data.handbooklvg))
		{
		   if (effecttj1star>0)
		    {
			    effecttj1star--;	
			    if (9>=effecttj1star)
		         {
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN7, false);     // 清除红色图鉴套装状态
					 owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN6, false);     // 清除橙色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN5, false);     // 清除紫色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN4, false);     // 清除绿色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN3, false);     // 清除黄色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN2, false);     // 清除蓝色图鉴套装状态
			         owner->showCurrentEffect(Cmd::USTATE_USER_TZ_TUJIAN1, false);     // 清除白色图鉴套装状态
				   	owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG, false); // 更新大波浪
					owner->showCurrentEffect(Cmd::USTATE_USER_TZ_DABOLANG2, false); // 更新大波浪
		        }  
		    }
	    }	
	}		
	//soke 清除星星套装效果 start
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (15 <= object->data.upgrade))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect15star>0)
			{
				effect15star--;	
				if (9>=effect15star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (14 <= object->data.upgrade))
		{
			if (effect14star>0)
			{
				effect14star--;	
				if (9>=effect14star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (13 <= object->data.upgrade))
		{
			if (effect13star>0)
			{
				effect13star--;	
				if (9>=effect13star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (12 <= object->data.upgrade))
		{
			if (effect12star>0)
			{
				effect12star--;
				if (9>=effect12star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (10 <= object->data.upgrade))
		{
			if (effect10star>0)
			{
				effect10star--;	
				if (9>=effect10star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (8 <= object->data.upgrade))
		{
			if (effect8star>0)
			{
				effect8star--;
				if (9>=effect8star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (5 <= object->data.upgrade))
		{
			if (effect5star>0)
			{
				effect5star--;
				if (9>=effect5star)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
		if (owner && object && (3 <= object->data.upgrade))
		{
			if (effect3star>0)
			{
				effect3star--;	
				if (9>=effect3star)
				{
		 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_THR_XING, false); // 清除3星状态
		    		owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIV_XING, false); // 清除5星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_EIG_XING, false); // 清除8星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TEN_XING, false); // 清除10星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_TWE_XING, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_THI_XING, false); // 清除13星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FOU_XING, false); // 清除14星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING, false); // 清除15星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_FIF_XING2, false); // 清除15星状态
				}
			}
		}
	}
	//soke 清除龙星套装效果 longxt
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (15 <= object->data.drastar))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect15longx>0)
			{
				effect15longx--;	
				if (9>=effect15longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING, false); // 清除5x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, false); // 清除8x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, false); // 清除11x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, false); // 清除13x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, false); // 清除15x龙星状态
				}
			}
		}
		if (owner && object && (13 <= object->data.drastar))
		{
			if (effect13longx>0)
			{
				effect13longx--;	
				if (9>=effect13longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING, false); // 清除5x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, false); // 清除8x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, false); // 清除11x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, false); // 清除13x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, false); // 清除15x龙星状态
				}
			}
		}
		if (owner && object && (11 <= object->data.drastar))
		{
			if (effect11longx>0)
			{
				effect11longx--;	
				if (9>=effect11longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING, false); // 清除5x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, false); // 清除8x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, false); // 清除11x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, false); // 清除13x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, false); // 清除15x龙星状态
				}
			}
		}
		if (owner && object && (8 <= object->data.drastar))
		{
			if (effect8longx>0)
			{
				effect8longx--;
				if (9>=effect8longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING, false); // 清除5x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, false); // 清除8x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, false); // 清除11x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, false); // 清除13x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, false); // 清除15x龙星状态
				}
			}
		}
		if (owner && object && (5 <= object->data.drastar))
		{
			if (effect5longx>0)
			{
				effect5longx--;
				if (9>=effect5longx)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_WU_XING, false); // 清除5x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BA_XING, false); // 清除8x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIYI_XING, false); // 清除11x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHISA_XING, false); // 清除13x龙星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHIWU_XING, false); // 清除15x龙星状态
				}
			}
		}
	}

	//soke 清除补天套装效果 butiant
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		// if (owner && object && (5 <= object->data.butian))
		// {
		// 	//soke 大于0就要开始减少，不然会持续增加次数
		// 	if (effect5butian>0)
		// 	{
		// 		effect5butian--;	
		// 		if (9>=effect5butian)
		// 		{
		// 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI, false); // 清除1x补天状态
		// 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, false); // 清除2x补天状态
		// 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, false); // 清除3x补天状态
		// 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, false); // 清除4x补天状态
		// 			owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, false); // 清除5x补天状态
		// 		}
		// 	}
		// }
		if (owner && object && (4 <= object->data.butian))
		{
			if (effect4butian>0)
			{
				effect4butian--;	
				if (9>=effect4butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI, false); // 清除1x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, false); // 清除2x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, false); // 清除3x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, false); // 清除4x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, false); // 清除5x补天状态
				}
			}
		}
		if (owner && object && (3 <= object->data.butian))
		{
			if (effect3butian>0)
			{
				effect3butian--;	
				if (9>=effect3butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI, false); // 清除1x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, false); // 清除2x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, false); // 清除3x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, false); // 清除4x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, false); // 清除5x补天状态
				}
			}
		}
		if (owner && object && (2 <= object->data.butian))
		{
			if (effect2butian>0)
			{
				effect2butian--;
				if (9>=effect2butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI, false); // 清除1x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, false); // 清除2x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, false); // 清除3x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, false); // 清除4x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, false); // 清除5x补天状态
				}
			}
		}
		if (owner && object && (1 <= object->data.butian))
		{
			if (effect1butian>0)
			{
				effect1butian--;
				if (9>=effect1butian)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN1_QI, false); // 清除1x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN2_QI, false); // 清除2x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN3_QI, false); // 清除3x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN4_QI, false); // 清除4x补天状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_BUTIAN5_QI, false); // 清除5x补天状态
				}
			}
		}
	}

	//soke 清除装备栏套装效果 
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (6 <= object->data.retain2))
		{
			if (effect6zhuangbei>0)
			{
				effect6zhuangbei--;	
				if (9>=effect6zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, false); // 清除2x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, false); // 清除3x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, false); // 清除4x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, false); // 清除5x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, false); // 清除6x装备栏状态
				}
			}
		}
		if (owner && object && (5 <= object->data.retain2))
		{
			if (effect5zhuangbei>0)
			{
				effect5zhuangbei--;	
				if (9>=effect5zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, false); // 清除2x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, false); // 清除3x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, false); // 清除4x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, false); // 清除5x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, false); // 清除6x装备栏状态
				}
			}
		}
		if (owner && object && (4 <= object->data.retain2))
		{
			if (effect4zhuangbei>0)
			{
				effect4zhuangbei--;	
				if (9>=effect4zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, false); // 清除2x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, false); // 清除3x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, false); // 清除4x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, false); // 清除5x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, false); // 清除6x装备栏状态
				}
			}
		}
		if (owner && object && (3 <= object->data.retain2))
		{
			if (effect3zhuangbei>0)
			{
				effect3zhuangbei--;	
				if (9>=effect3zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, false); // 清除2x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, false); // 清除3x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, false); // 清除4x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, false); // 清除5x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, false); // 清除6x装备栏状态
				}
			}
		}
		if (owner && object && (2 <= object->data.retain2))
		{
			if (effect2zhuangbei>0)
			{
				effect2zhuangbei--;	
				if (9>=effect2zhuangbei)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG2_QI, false); // 清除2x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG3_QI, false); // 清除3x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG4_QI, false); // 清除4x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG5_QI, false); // 清除5x装备栏状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_ZHUANG6_QI, false); // 清除6x装备栏状态
				}
			}
		}
	}

	//soke 清除神器套装效果 shenqit
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (7 <= object->data.retain3))
		{
			if (effect7shenqi>0)
			{
				effect7shenqi--;	
				if (9>=effect7shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (6 <= object->data.retain3))
		{
			if (effect6shenqi>0)
			{
				effect6shenqi--;	
				if (9>=effect6shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (5 <= object->data.retain3))
		{
			if (effect5shenqi>0)
			{
				effect5shenqi--;	
				if (9>=effect5shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (4 <= object->data.retain3))
		{
			if (effect4shenqi>0)
			{
				effect4shenqi--;	
				if (9>=effect4shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (3 <= object->data.retain3))
		{
			if (effect3shenqi>0)
			{
				effect3shenqi--;	
				if (9>=effect3shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (2 <= object->data.retain3))
		{
			if (effect2shenqi>0)
			{
				effect2shenqi--;	
				if (9>=effect2shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
		if (owner && object && (1 <= object->data.retain3))
		{
			if (effect1shenqi>0)
			{
				effect1shenqi--;	
				if (9>=effect1shenqi)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN1_QI, false); // 清除1x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN2_QI, false); // 清除2x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN3_QI, false); // 清除3x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN4_QI, false); // 清除4x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN5_QI, false); // 清除5x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN6_QI, false); // 清除6x神器状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_SHEN7_QI, false); // 清除7x神器状态
				}
			}
		}
	}

	// //soke 第二灵魂系统 清除开始-----------------------------------------------------------------
	// if (owner&&object && (object->data.bind && object->data.kind & 16 && 60 == object->data.addpdef && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
	// {//soke 大于0就要开始减少，不然会持续增加次数
	// 	if (effect21linghun>0)
	// 	{
	// 		effect21linghun--;	
	// 		if (7>=effect21linghun)
	// 	    { //物理防御力增强
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, false);  // 清除第二灵魂状态
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, false); // 清除第二灵魂物理防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, false); // 清除第二灵魂魔法防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, false); // 清除第二灵魂物理攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, false); // 清除第二灵魂魔法攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, false); // 清除第二灵魂增加生命加成
	// 	    }
	// 	}
	// }
	// if (owner&&object && (object->data.bind && object->data.kind & 16 && 60 == object->data.addmdef && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
	// {//soke 大于0就要开始减少，不然会持续增加次数
	// 	if (effect22linghun>0)
	// 	{
	// 		effect22linghun--;	
	// 		if (7>=effect22linghun)
	// 	    { //魔法防御力增强
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, false);  // 清除第二灵魂状态
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, false); // 清除第二灵魂物理防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, false); // 清除第二灵魂魔法防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, false); // 清除第二灵魂物理攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, false); // 清除第二灵魂魔法攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, false); // 清除第二灵魂增加生命加成
	// 	    }
	// 	}
	// }
	// if (owner&&object && (object->data.bind && object->data.kind & 16 && 120 == object->data.addpdam && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
	// {//soke 大于0就要开始减少，不然会持续增加次数
	// 	if (effect23linghun>0)
	// 	{
	// 		effect23linghun--;	
	// 		if (7>=effect23linghun)
	// 	    { //增加物理攻击力
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, false);  // 清除第二灵魂状态
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, false); // 清除第二灵魂物理防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, false); // 清除第二灵魂魔法防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, false); // 清除第二灵魂物理攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, false); // 清除第二灵魂魔法攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, false); // 清除第二灵魂增加生命加成
	// 	    }
	// 	}
	// }
	// if (owner&&object && (object->data.bind && object->data.kind & 16 && 120 == object->data.addmdam && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
	// {//soke 大于0就要开始减少，不然会持续增加次数
	// 	if (effect24linghun>0)
	// 	{
	// 		effect24linghun--;	
	// 		if (7>=effect24linghun)
	// 	    { //增加魔法攻击力
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, false);  // 清除第二灵魂状态
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, false); // 清除第二灵魂物理防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, false); // 清除第二灵魂魔法防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, false); // 清除第二灵魂物理攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, false); // 清除第二灵魂魔法攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, false); // 清除第二灵魂增加生命加成
	// 	    }
	// 	}
	// }
	// if (owner&&object && (object->data.bind && object->data.kind & 16 && 100 == object->data.addhp && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155)))
	// {//soke 大于0就要开始减少，不然会持续增加次数
	// 	if (effect25linghun>0)
	// 	{
	// 		effect25linghun--;	
	// 		if (7>=effect25linghun)
	// 	    { //增加生命值
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN, false);  // 清除第二灵魂状态
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN1, false); // 清除第二灵魂物理防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN2, false); // 清除第二灵魂魔法防御加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN3, false); // 清除第二灵魂物理攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN4, false); // 清除第二灵魂魔法攻击加成
	// 		   owner->showCurrentEffect(Cmd::USTATE_DI_ER_LING_HUN5, false); // 清除第二灵魂增加生命加成
	// 	    }
	// 	}
	// }
	// //soke 第二灵魂系统 清除结束-----------------------------------------------------------------
	
	//soke 清除15星斗魂套装
	if(owner && object && object->data.bind && ( (object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155) )
	{
        if (owner && object && (15 <= object->data.douhun))
	    {
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect15douhun>0)
			{
				effect15douhun--;	
				if (9>=effect15douhun)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, false); // 清除6星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, false); // 清除9星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, false); // 清除15星状态
				}
			}
	    }
	//soke 清除12星斗魂套装
        if (owner && object && (12 <= object->data.douhun))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect12douhun>0)
			{
				effect12douhun--;	
				if (9>=effect12douhun)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, false); // 清除6星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, false); // 清除9星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, false); // 清除15星状态
				}
			}
		}
	
	//soke 清除9星斗魂套装
        if (owner && object && (9 <= object->data.douhun))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect9douhun>0)
			{
				effect9douhun--;	
				if (9>=effect9douhun)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, false); // 清除6星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, false); // 清除9星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, false); // 清除15星状态
				}
			}
		}
	//soke 清除6星斗魂套装
        if (owner && object && (6 <= object->data.douhun))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect6douhun>0)
			{
				effect6douhun--;	
				if (9>=effect6douhun)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, false); // 清除6星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, false); // 清除9星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, false); // 清除15星状态
				}
			}
		}
	//soke 清除3星斗魂套装
        if (owner && object && (3 <= object->data.douhun))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect3douhun>0)
			{
				effect3douhun--;	
				if (9>=effect3douhun)
				{
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN3, false); // 清除3星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN6, false); // 清除6星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN9, false); // 清除9星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN12, false); // 清除12星状态
					owner->showCurrentEffect(Cmd::USTATE_ULTRA_DOUHUN15, false); // 清除15星状态
				}
			}
		}
	}
	//soke 清除神龙宝宝镶嵌套装效果 ///////////////////////////////////////////////////////////////////////////////////////////
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{		
		
        if (owner && object && (6 <= object->data.longwanglv))
		{
            //soke 大于0就要开始减少，不然会持续增加次数
			if (effect6shenlong>0)
			{
				effect6shenlong--;	
				if (9>=effect6shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
        if (owner && object && (5 <= object->data.longwanglv))
		{
			if (effect5shenlong>0)
			{
				effect5shenlong--;	
				if (9>=effect5shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
        if (owner && object && (4 <= object->data.longwanglv))
		{
			if (effect4shenlong>0)
			{
				effect4shenlong--;	
				if (9>=effect4shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
		if (owner && object && (3 <= object->data.longwanglv))
		{
			if (effect3shenlong>0)
			{
				effect3shenlong--;	
				if (9>=effect3shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
		if (owner && object && (2 <= object->data.longwanglv))
		{
			if (effect2shenlong>0)
			{
				effect2shenlong--;
				if (9>=effect2shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
		if (owner && object && (1 <= object->data.longwanglv))
		{
			if (effect1shenlong>0)
			{
				effect1shenlong--;
				if (9>=effect1shenlong)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV1, false); // 清除小龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV2, false); // 清除金龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV3, false); // 清除苍龙套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV4, false); // 清除巨龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV5, false); // 清除神苍龙套装状态
                    owner->showCurrentEffect(Cmd::USTATE_USER_LONGWANG_LV6, false); // 清除神巨龙套装状态
				}
			}
		}
	}

	//soke 清除逆天进阶装备套装效果 /////////////////////////////////////////////
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (100 <= object->data.nitianlv))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect5nitian>0)
			{
				effect5nitian--;	
				if (9>=effect5nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1, false); // 清除15阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, false); // 清除30阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, false); // 清除50阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, false); // 清除80阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, false); // 清除100阶逆天进阶状态
				}
			}
		}
		if (owner && object && (80 <= object->data.nitianlv))
		{
			if (effect4nitian>0)
			{
				effect4nitian--;	
				if (9>=effect4nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1, false); // 清除15阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, false); // 清除30阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, false); // 清除50阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, false); // 清除80阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, false); // 清除100阶逆天进阶状态
				}
			}
		}
		if (owner && object && (50 <= object->data.nitianlv))
		{
			if (effect3nitian>0)
			{
				effect3nitian--;	
				if (9>=effect3nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1, false); // 清除15阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, false); // 清除30阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, false); // 清除50阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, false); // 清除80阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, false); // 清除100阶逆天进阶状态
				}
			}
		}
		if (owner && object && (30 <= object->data.nitianlv))
		{
			if (effect2nitian>0)
			{
				effect2nitian--;
				if (9>=effect2nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1, false); // 清除15阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, false); // 清除30阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, false); // 清除50阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, false); // 清除80阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, false); // 清除100阶逆天进阶状态
				}
			}
		}
		if (owner && object && (15 <= object->data.nitianlv))
		{
			if (effect1nitian>0)
			{
				effect1nitian--;
				if (9>=effect1nitian)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV1, false); // 清除15阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV2, false); // 清除30阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV3, false); // 清除50阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV4, false); // 清除80阶逆天进阶状态
					owner->showCurrentEffect(Cmd::USTATE_USER_NITIAN_LV5, false); // 清除100阶逆天进阶状态
				}
			}
		}
	}
	
	//soke 清除神魂宝石(斗战圣佛)套装效果 
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{		
		if (owner && object && (100 <= object->data.douzhansflv))
		{
            //soke 大于0就要开始减少，不然会持续增加次数
			if (effect4douzhansf>0)
			{
				effect4douzhansf--;	
				if (9>=effect4douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV1, false); // 清除25阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, false); // 清除50阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, false); // 清除75阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, false); // 清除100阶神魂宝石(斗战圣佛)套装状态
				}
			}
		}
		if (owner && object && (75 <= object->data.douzhansflv))
		{
			if (effect3douzhansf>0)
			{
				effect3douzhansf--;	
				if (9>=effect3douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV1, false); // 清除25阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, false); // 清除50阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, false); // 清除75阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, false); // 清除100阶神魂宝石(斗战圣佛)套装状态
				}
			}
		}
		if (owner && object && (50 <= object->data.douzhansflv))
		{
			if (effect2douzhansf>0)
			{
				effect2douzhansf--;
				if (9>=effect2douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV1, false); // 清除25阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, false); // 清除50阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, false); // 清除75阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, false); // 清除100阶神魂宝石(斗战圣佛)套装状态
				}
			}
		}
		if (owner && object && (25 <= object->data.douzhansflv))
		{
			if (effect1douzhansf>0)
			{
				effect1douzhansf--;
				if (9>=effect1douzhansf)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV1, false); // 清除25阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV2, false); // 清除50阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV3, false); // 清除75阶神魂宝石(斗战圣佛)套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_DOUZHANSF_LV4, false); // 清除100阶神魂宝石(斗战圣佛)套装状态
				}
			}
		}
	}

	//soke 清除暗影宝石套装效果 
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (100 <= object->data.aystonelv))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect5aystone>0)
			{
				effect5aystone--;	
				if (9>=effect5aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1, false); // 清除20阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, false); // 清除40阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, false); // 清除60阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, false); // 清除80阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, false); // 清除100阶暗影宝石套装状态
				}
			}
		}
		if (owner && object && (80 <= object->data.aystonelv))
		{
			if (effect4aystone>0)
			{
				effect4aystone--;	
				if (9>=effect4aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1, false); // 清除20阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, false); // 清除40阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, false); // 清除60阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, false); // 清除80阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, false); // 清除100阶暗影宝石套装状态
				}
			}
		}
		if (owner && object && (60 <= object->data.aystonelv))
		{
			if (effect3aystone>0)
			{
				effect3aystone--;	
				if (9>=effect3aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1, false); // 清除20阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, false); // 清除40阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, false); // 清除60阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, false); // 清除80阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, false); // 清除100阶暗影宝石套装状态
				}
			}
		}
		if (owner && object && (40 <= object->data.aystonelv))
		{
			if (effect2aystone>0)
			{
				effect2aystone--;
				if (9>=effect2aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1, false); // 清除20阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, false); // 清除40阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, false); // 清除60阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, false); // 清除80阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, false); // 清除100阶暗影宝石套装状态
				}
			}
		}
		if (owner && object && (20 <= object->data.aystonelv))
		{
			if (effect1aystone>0)
			{
				effect1aystone--;
				if (9>=effect1aystone)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV1, false); // 清除20阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV2, false); // 清除40阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV3, false); // 清除60阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV4, false); // 清除80阶暗影宝石套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_AYSTONE_LV5, false); // 清除100阶暗影宝石套装状态
				}
			}
		}
	}

	//soke 清除龙凤吟套装效果 
	if (owner && object && (object->data.needlevel >= 70) && (object->data.bind) && (!object->data.kind & 4 || object->data.kind & 16) && ((object->base->kind >= 101 && object->base->kind <= 111) || (object->base->kind >= 113 && object->base->kind <= 118) || object->base->kind == 136 || object->base->kind == 137 || object->base->kind == 155))
	{
		if (owner && object && (100 <= object->data.retain24))
		{
			//soke 大于0就要开始减少，不然会持续增加次数
			if (effect5lfy>0)
			{
				effect5lfy--;	
				if (9>=effect5lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1, false); // 清除8级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, false); // 清除10级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, false); // 清除12级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, false); // 清除14级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, false); // 清除16级龙凤吟套装状态
				}
			}
		}
		if (owner && object && (80 <= object->data.retain24))
		{
			if (effect4lfy>0)
			{
				effect4lfy--;	
				if (9>=effect4lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1, false); // 清除8级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, false); // 清除10级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, false); // 清除12级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, false); // 清除14级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, false); // 清除16级龙凤吟套装状态
				}
			}
		}
		if (owner && object && (60 <= object->data.retain24))
		{
			if (effect3lfy>0)
			{
				effect3lfy--;	
				if (9>=effect3lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1, false); // 清除8级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, false); // 清除10级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, false); // 清除12级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, false); // 清除14级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, false); // 清除16级龙凤吟套装状态
				}
			}
		}
		if (owner && object && (40 <= object->data.retain24))
		{
			if (effect2lfy>0)
			{
				effect2lfy--;
				if (9>=effect2lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1, false); // 清除8级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, false); // 清除10级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, false); // 清除12级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, false); // 清除14级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, false); // 清除16级龙凤吟套装状态
				}
			}
		}
		if (owner && object && (20 <= object->data.retain24))
		{
			if (effect1lfy>0)
			{
				effect1lfy--;
				if (9>=effect1lfy)
				{
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV1, false); // 清除8级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV2, false); // 清除10级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV3, false); // 清除12级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV4, false); // 清除14级龙凤吟套装状态
					owner->showCurrentEffect(Cmd::USTATE_USER_LFY_LV5, false); // 清除16级龙凤吟套装状态
				}
			}
		}
	}


	if(ret && object && object->base->kind == ItemType_Amulet && owner->scene->isIncScene())
	{
		std::ostringstream os;
		os << "name=" << object->data.maker;
		os << " pos=" << object->data.durpoint << "," << object->data.dursecond;
		bzero(object->data.maker,sizeof(object->data.maker));
		object->data.durpoint=0;
		object->data.dursecond=0;
		Cmd::stAddObjectPropertyUserCmd ret;
		ret.byActionType = Cmd::EQUIPACTION_REFRESH;
		bcopy(&object->data, &ret.object, sizeof(t_Object));
		owner->sendCmdToMe(&ret, sizeof(ret));				
		Gm::gomap(owner, os.str().c_str());
	}
	needRecalc=true;

	return ret;
}

/************************************************************************/
/* 
\brief 检测物品的装备职业是否符合当前玩家的职业
changejob 玩家职业
EquipType 物品类型
*/
/************************************************************************/
bool EquipPack::IsJobEquip(SceneUser *pUser,DWORD changejob, DWORD EquipType)
{
	//soke 戒指或者项链是任何职业都可以装备的
	//23包裹（无忧袋）、30活力之源、41月光宝盒、52双倍经验、53荣誉之星、61护身符、62补蓝、80护身符、101-103服装、113头盔、114腰带、115护腕、116靴子、117项链、
	//118戒指、119低级时装、120采集手套、124高级时装、马脚、马绳、马鞍、马甲、马镫、马铃铛、80护身符 都没有职业限制 	法宝

	if(EquipType == ItemType_Pack || EquipType == ItemType_Tonic || EquipType == ItemType_Change || EquipType == ItemType_DoubleExp || EquipType == ItemType_Honor 
	|| EquipType == ItemType_DoubleExp|| EquipType == ItemType_Amulet || EquipType == ItemType_GreatLeechdomMp || EquipType == ItemType_Amulet || EquipType == ItemType_ClothBody
	|| EquipType == ItemType_FellBody || EquipType == ItemType_MetalBody || EquipType == ItemType_Helm || EquipType == ItemType_Caestus || EquipType ==  ItemType_Cuff 
	|| EquipType ==  ItemType_Shoes || EquipType == ItemType_Necklace || EquipType == ItemType_Fing || EquipType == ItemType_FashionBody|| EquipType == ItemType_Flower 
	|| EquipType == ItemType_HighFashionBody|| EquipType == ItemType_XHighFashionBody || EquipType == ItemType_SHighFashionBody || EquipType == ItemType_HorseShoe || EquipType == ItemType_HorseRope || EquipType == ItemType_HorseSaddle
	|| EquipType == ItemType_HorseSafe|| EquipType == ItemType_HorseIron || EquipType == ItemType_HorseFashion || EquipType == ItemType_Amulets || EquipType == 72|| EquipType == 90
	|| EquipType == 91 || EquipType == 99 || (EquipType >= 88 && EquipType <= 89)|| (EquipType >= 93 && EquipType <= 98)|| (EquipType >= ItemType_handbookitem1 && EquipType <= ItemType_handbookitem10)
	|| (EquipType >= ItemType_Fabao && EquipType <= ItemType_Fabaofj12) || (EquipType >= ItemType_SEXX1 && EquipType <= ItemType_SEXX12) || EquipType == 71
	|| (EquipType >= ItemType_YUANSHEN && EquipType <= ItemType_YUANSHENFJ6)|| EquipType == ItemType_Gift)
	{
		return true;
	}	
	

	if(changejob>0 && changejob<=9)
	{
		switch(changejob)
		{
		case 1:		//战士
			{
				if( EquipType == ItemType_Blade || EquipType == ItemType_Sword || EquipType == ItemType_Axe || EquipType == ItemType_Hammer || EquipType == ItemType_Shield || EquipType == ItemType_BladeFlag )
				{
					return true;
				}
				else
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：战士，不能装备其它职业武器！"); 
					return false;
				}
			}
			break;
		case 2:		//弓手
			{
				if( EquipType == ItemType_Crossbow || EquipType == ItemType_Arrow || EquipType == ItemType_CrossFlag)
				{
					return true;
				}
				else
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：弓手，不能装备其它职业武器！"); 
					return false;
				}
			}
			break;
		case 3:		//法师
		{
				if( EquipType == ItemType_Staff || EquipType == ItemType_StaffFlag)
				{
					return true;
				}
				else
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：法师，不能装备其它职业武器！"); 
					return false;
				}
			}
			break;
		case 4:		//召唤
			{
				if( EquipType == ItemType_Stick || EquipType == ItemType_StickFlag)
				{
					return true;
				}
				else
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：召唤，不能装备其它职业武器！"); 
					return false;
				}
			}
			break;
			case 5:		//天仙
			{
				if( EquipType == ItemType_Fan || EquipType == ItemType_Flag)
				{
					return true;
				}
				else
				{
					Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：天仙，不能装备其它职业武器！"); 
					return false;
				}
			}
			break;
			case 6:		//刺客
				{
					if( EquipType == ItemType_Dagger || EquipType == ItemType_DaggerFlag)
					{
						return true;
					}
					else
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：刺客，不能装备其它职业武器！"); 
						return false;
					}
				}
				break;
			case 7:		//卫士
				{
					if( EquipType == ItemType_NewSword || EquipType == ItemType_breastplate|| EquipType == ItemType_SwordFlag)
					{
						return true;
					}
					else
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：卫士，不能装备其它职业武器！"); 
						return false;
					}
				}
				break;
			case 8:		//枪
				{
					if( EquipType == ItemType_LongGun || EquipType == ItemType_Handgun)
					{
						return true;
					}
					else
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_SYS, "你的职业是：枪系，不能装备其它职业武器！"); 
						return false;
					}
				}
				break;
			case 9:		//全职业
			{
				if( (EquipType >= 104 && EquipType <= 111) || (EquipType >= 141 && EquipType <= 147) || (EquipType >= 136 && EquipType <= 138) || EquipType == 155 || 156 )
				{
					return true;
				}
			}
			break;
		default:
			break;
		}
	}

	return false;
}

/**
 * \brief 检查物品
 * \param pUser 角色
 * \param object 物品对象
 * \param x 横坐标
 * \param y 纵坐标
 * \return true 装备有此物品
 */
bool EquipPack::checkAdd(SceneUser *pUser,zObject *ob,WORD x,WORD y)
{
	if (ob && ob->base->setpos == y && (y == Cmd::EQUIPCELLTYPE_MAKE || y == Cmd::EQUIPCELLTYPE_PACKAGE)) {
		int pos = ((y- Cmd::EQUIPCELLTYPE_PACKAGE) << 1)  + x;
		if (pos >= 0 && pos < 10 && packs[pos] && !packs[pos]->empty()) return false;
	}
	
	zObject* tmp;
	bool ret = getObjectByZone(&tmp, x, y);
	if (ob==NULL) return true;

	//soke 装备前先检查下职业是否符合该装备的职业限定
	if(!IsJobEquip(pUser,pUser->charbase.changejob, ob->base->kind))
	{
		Zebra::logger->debug("ID:%u 玩家:%s 试图装备自己职业无法装备的物品:%s" ,pUser->charbase.id ,pUser->charbase.name, ob->data.strName); //桃子增加转生装备穿戴限制
		Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "该武器不适合你当前的职业！");
		return false;
	}

	using namespace Cmd;
	if(!(pUser->charbase.type&ob->base->sex) || pUser->charbase.level<ob->data.needlevel || pUser->charbase.round<ob->data.round)
		return false;

	if (ob->base->setpos && ob->base->setpos == y) 
	{
		switch(ob->base->kind)
		{
			case ItemType_DoubleExp:
				{
					if(ob->data.dur<= 60)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "剩余时间太短,不能装备"); 
						return false;
					}
					if(doubleexp_obj_time/86400 == SceneTimeTick::currentTime.sec()/86400)
					{
						if(doubleexp_obj != ob->data.qwThisID)
						{
							Channel::sendSys(pUser, Cmd::INFO_TYPE_GAME, "一天只能使用一个双倍经验物品！");
							return false;
						}
					}
					else
					{
						doubleexp_obj = ob->data.qwThisID;
						doubleexp_obj_time = SceneTimeTick::currentTime.sec(); 
					}
				}
				break;
			case ItemType_FashionBody:
				{
					if(ob->data.dur<= 1)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "剩余时间太短,不能装备"); 
						return false;
					}
				}
				break;
//////////////////////////////////////////////////////////				
		case ItemType_HighFashionBody:
			{
				if(ob->data.dur <= 1)
				{
					Channel::sendSys( pUser,Cmd::INFO_TYPE_FAIL,"剩余时间太短,不能装备");
					return false;
				}
			}
////////////////////////////////////////////////////////
			case ItemType_Tonic:
			case ItemType_Amulet:
				{
					if(ob->data.dur<= 60)
					{
						Channel::sendSys(pUser, Cmd::INFO_TYPE_FAIL, "剩余时间太短,不能装备"); 
						return false;
					}
				}
				break;
			default:
				break;
		}
		//双倍经验道具一天用一次
		if ( (ob->base->setpos == EQUIPCELLTYPE_HANDL && container[HANDR] && container[HANDR]->base->kind != ob->base->needobject && ob->base->kind != container[HANDR]->base->needobject) ||(ob->base->setpos == EQUIPCELLTYPE_HANDR && container[HANDL]  && container[HANDL]->base->kind  != ob->base->needobject && ob->base->kind != container[HANDL]->base->needobject ) ) 
		{
			
			zObject* obj = container[HANDR] ;
			if (ob->base->setpos == EQUIPCELLTYPE_HANDR) obj = container[HANDL] ;

			if (!obj) return ret;
			
			if (pUser->packs.uom.space(pUser) > 0 ) 
			{
				pUser->packs.removeObject(obj, true, false); //notify but not delete

/*				
				obj->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_COMMON;
				pUser->packsaddObject(obj, true);
*/
				pUser->packs.addObject(obj, true, AUTO_PACK);

				Cmd::stAddObjectPropertyUserCmd ret1;
				ret1.byActionType = Cmd::EQUIPACTION_OBTAIN;
				bcopy(&obj->data, &ret1.object, sizeof(t_Object));
				pUser->sendCmdToMe(&ret1, sizeof(ret1));

/*			
				if (ob->base->setpos == EQUIPCELLTYPE_HANDL) {
					container[HANDR] = NULL;
				}else {
					container[HANDL] = NULL;
				}
*/
				return ret;
			}
			return false;
		}
						
		return ret;
	}
	
	return false;
}

/**
 * \brief 根据位置大小获取物品
 * \param ret 返回的物品对象
 * \param x 横坐标
 * \param y 纵坐标
 * \param width 宽度
 * \param height 高度
 * \return true 成功返回
 */
bool EquipPack::getObjectByZone(zObject **ret,WORD x,WORD y)
{
	if (process_extra_get(ret, x, y)) return true;
	if(y>Cmd::EQUIPCELLTYPE_MAX)
	{
		return false;
	}
	return Package::getObjectByZone(ret, x, y);
}

/**
 * \brief 经验绑定
 * \param user 角色
 * \param exp 绑定到装备身上的经验数量
 * \param force 不判断几率强制绑定
 */
void EquipPack::obtain_exp(SceneUser* user, DWORD exp, bool force)
{
	if (force || zMisc::selectByPercent(20)) {
		int index = zMisc::randBetween(0, 15);
		int current = index;
		do {
			if (container[current] != NULL)
			{
				switch ( container[current]->base->kind)
				{
					case ItemType_Blade:		    //104代表武术刀类武器
					case ItemType_Sword :	        //105代表武术剑类武器
					case ItemType_Axe:	            //106代表武术斧类武器
					case ItemType_Hammer:	        //107代表武术斧类武器
					case ItemType_Staff:		    //108代表法术杖类武器
					case ItemType_Crossbow:	        //109代表箭术弓类武器
					case ItemType_Fan:	            //110代表美女扇类
					case ItemType_Stick:	        //111代表召唤棍类武器			
					case ItemType_Necklace:	        //117代表角色项链类
					case ItemType_Fing:		        //118代表角色戒指类
					case ItemType_ClothBody:		//101代表布质加生命类服装
					case ItemType_FellBody:		    //102代表皮甲加魔防类服装
					case ItemType_MetalBody:		//103代表金属铠甲加物防类服装
					case ItemType_Shield:	        //112代表盾牌类
					case ItemType_Helm:		        //113代表角色头盔类
					case ItemType_Caestus:	        //114代表角色腰带类
					case ItemType_Cuff:		        //115代表角色护腕类
					case ItemType_Shoes:		    //116代表角色鞋子类
					case ItemType_HorseShoe:        //130代表坐骑马脚类
			        case ItemType_HorseRope:        //131代表坐骑马绳类
			        case ItemType_HorseSaddle:      //132代表坐骑马鞍类
			        case ItemType_HorseSafe:        //133代表坐骑马甲类
			        case ItemType_HorseIron:        //134代表坐骑马镫类
					case ItemType_Dagger:	        //136代表利刃
					case ItemType_NewSword:	        //137代表卫士剑
	        		case ItemType_breastplate:	    //138代表护心镜
	        		case ItemType_StaffFlag:		//141代表战旗(法师)
	        		case ItemType_CrossFlag:	    //142代表战旗(弓手)
	        		case ItemType_Flag:	            //143代表战旗(仙术)
	        		case ItemType_StickFlag:	    //144代表战旗(召唤)
	        		case ItemType_BladeFlag:	    //145代表战旗(战士)
	        		case ItemType_DaggerFlag:	    //146代表战旗(刺客)
	        		case ItemType_SwordFlag:        //147代表战旗(卫士)
	        		case ItemType_LongGun:          //155代表长枪
	        		case ItemType_Handgun:          //156代表短枪
						{
							container[current] ->data.exp += static_cast<int>(exp*0.05);
							Cmd::stObjectExpUserCmd cmd;
							cmd.id = container[current]->data.qwThisID;
							cmd.exp = container[current]->data.exp;
							user->sendCmdToMe(&cmd , sizeof(cmd));
						}
						break;
					default:
						break;
				}
				break;
			}

		}while ( ( current = (++current % Cmd::EQUIPCELLTYPE_MAX) ) != index ) ;
	}		
}

/**
 * \brief 箭支消耗数量是以耐久度计算的,当耐久为0时删除箭桶
 * \param pThis 主人
 * \param kind 物品类型
 * \param num 消耗数量
 * \author whj 
 * \return 消耗是否成功
 */
bool EquipPack::skillCheckReduceObject(SceneUser* pThis, DWORD kind, WORD num)
{
/*
	std::set<zObject *>::iterator it = allset.begin();
	int count=0;
	while (it != allset.end()) 
*/
	int count=0;	

	for (int i=0; i<16; ++i) 
	{
		if (!container[i]) continue;
		
		switch(kind)
		{
			case BOW_ARROW_ITEM_TYPE:
				{
					if ( container[i]->base->kind == kind) 
					{
						if((short)container[i]->data.dur < num)
						{
							return false;
						}
						else
						{
							return true;
						}
					}
					break;
				}
			default:
				{
					if ( container[i]->base->kind == kind) 
					{
						count++;
					}
				}
		}
//		++it;		
	}
	if (count>=num) return true;
	return false;
}

/**
 * \brief 物品消耗处理（用于技能的物品消耗）
 * \param pThis 主人
 * \param kind 物品的类型
 * \param num 消耗物品的数目
 * \author whj 
 * \return 消耗是否成功
 */
bool EquipPack::skillReduceObject(SceneUser* pThis, DWORD kind, WORD num)
{
//	std::set<zObject *>::iterator it = allset.begin();
	int count =0;
//	while (it != allset.end()) 
	for (int i=0; i<16; ++i) 
	{
		if (!container[i]) continue;
		
		switch(kind)
		{
			case BOW2_ARROW_ITEM_TYPE: //soke 战旗（弓手）修复
				{
					if ( container[i]->base->kind == kind) 
					{
						if((short)container[i]->data.dur <= num)
						{
							zObject::logger(container[i]->createid,container[i]->data.qwThisID,container[i]->data.strName,container[i]->data.dwNum,1,0,pThis->id,pThis->name,0,NULL,"技能消耗",NULL,0,0);
							pThis->packs.removeObject(container[i]);
							return true;
						}
						else
						{
							//container[i]->data.dur -=num;
							Cmd::stDurabilityUserCmd std;
							std.dwThisID = container[i]->data.qwThisID;
							std.dwDur = container[i]->data.dur;
							pThis->sendCmdToMe(&std , sizeof(std));
							return true;
						}
					}
					break;
				}
				
			case BOW_ARROW_ITEM_TYPE:
				{
					if ( container[i]->base->kind == kind) 
					{
						if((short)container[i]->data.dur <= num)
						{
/*						
							zObject * obj = *it;
							Cmd::stRemoveObjectPropertyUserCmd rm;
							rm.qwThisID=obj->data.qwThisID;
							pThis->sendCmdToMe(&rm,sizeof(rm));
							zObject::logger((*it)->createid,(*it)->data.qwThisID,(*it)->data.strName,(*it)->data.dwNum,pThis->id,pThis->name,0,NULL,"技能消耗");
							pThis->packs.rmObject(obj);
							SAFE_DELETE(obj);
*/
							zObject::logger(container[i]->createid,container[i]->data.qwThisID,container[i]->data.strName,container[i]->data.dwNum,1,0,pThis->id,pThis->name,0,NULL,"技能消耗",NULL,0,0);
							pThis->packs.removeObject(container[i]);
							return true;
						}
						else
						{
						////////////////////////////////////
						//soke 取消箭支消耗
						//	container[i]->data.dur -=num;
						///////////////////////////////////
							Cmd::stDurabilityUserCmd std;
							std.dwThisID = container[i]->data.qwThisID;
							std.dwDur = container[i]->data.dur;
							pThis->sendCmdToMe(&std , sizeof(std));
							//Zebra::logger->debug("消耗箭支id=%u,thisid=%u,dur=%u",id ,std.dwThisID , std.dwDur);
							return true;
						}
					}
					break;
				}
			default:
				{
					if ( container[i]->base->kind == kind) 
					{
/*
						std::set<zObject *>::iterator temp = it;
						it++;
						zObject * obj = *temp;
						Cmd::stRemoveObjectPropertyUserCmd rm;
						rm.qwThisID=obj->data.qwThisID;
						pThis->sendCmdToMe(&rm,sizeof(rm));
						zObject::logger((*it)->createid,(*it)->data.qwThisID,(*it)->data.strName,(*it)->data.dwNum,pThis->id,pThis->name,0,NULL,"技能消耗");
						count++;
						pThis->packs.rmObject(obj);
						SAFE_DELETE(obj);
*/						
						zObject::logger(container[i]->createid,container[i]->data.qwThisID,container[i]->data.strName,container[i]->data.dwNum,1,0,pThis->id,pThis->name,0,NULL,"技能消耗",NULL,0,0);
						pThis->packs.removeObject(container[i]);
						if (count >= num) return true;
						continue;
					}
				}
		}
		//++it;		
	}
	if (count>=num) return true;
	
	return false;
}


/**
 * \brief 构造函数
 * \param ob 物品对象
 * \param type 类型
 */
ObjectPack::ObjectPack(zObject* ob, int type, bool consume)	:
	Package(type, ob->data.qwThisID, ob->data.maxpdamage, ob->data.maxmdamage), 
	_ob(ob), _cosume_by_time(consume), _one_min(60, SceneTimeTick::currentTime)
{
	if (_ob && _ob->data.maxdur ==0) //应对之前没有耐久的物品
	{
		_ob->data.maxdur = 43200;
		_ob->data.dur = 43200;
	}
	if (ob->base->setpos == Cmd::EQUIPCELLTYPE_PACKAGE  && !_ob->data.bind) {
		_ob->data.bind = 1;
		_ob->data.color = time(NULL);
	}
}

/**
 * \brief 析构函数
 */
ObjectPack::~ObjectPack()
{
	//SAFE_DELETE(_ob);
}

/*
bool ObjectPack::add(zObject *object ,bool find)
{
	if (find && (!_ob || _ob->data.dur == 0)) {
		//only can takeout
		return false;
	}
	return Package::add(object, find);
}
// */
bool ObjectPack::checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y)
{
	if (!_ob || _ob->data.dur == 0) {
		//only can takeout
		return object == NULL;
	}
	if(object && _ob->data.qwThisID == object->data.qwThisID)
	{
		Zebra::logger->debug("把自己添加到自己的包裹%s(%d),%s(%u)",pUser->name,pUser->id,object->data.strName,object->data.qwThisID);
		return false;
	}

	return Package::checkAdd(pUser, object, x, y);
}

void ObjectPack::consume_dur_by(SceneUser* user, const zRTime& current)
{
/*
	if (_cosume_by_time && _ob && _ob->data.dur > 0 && _one_min(current) ) {
		
		--_ob->data.dur;
		Cmd::stDurabilityUserCmd cmd;
		cmd.dwThisID = _ob->data.qwThisID;
		cmd.dwDur = _ob->data.dur;
		user->sendCmdToMe(&cmd , sizeof(cmd));
	}
*/
}

/**
 * \brief 构造函数初始化人物主包裹
 */
MainPack::MainPack():Package(Cmd::OBJECTCELLTYPE_COMMON,0,MainPack::WIDTH,MainPack::HEIGHT)
{
	gold=NULL;
}

/**
 * \brief 析构函数
 */
MainPack::~MainPack()
{
	SAFE_DELETE(gold);
}

/**
 * \brief 向包裹中添加对象
 * \param object 物品对象
 * \param find 寻找标志
 * \return true 添加成功 ， false 添加失败
 */
bool MainPack::add(zObject *object ,bool find)
{
	if(object && object->base->kind==ItemType_Money)
	{
		if(gold)
		{
			return false;
		}
		else
		{
			gold=object;
/*
			gold->data.pos.dwLocation=Cmd::OBJECTCELLTYPE_COMMON;
			gold->data.pos.dwTableID=0;
			gold->data.pos.x=(WORD)-1;
			gold->data.pos.y=(WORD)-1;
*/
			gold->data.pos = stObjectLocation(Cmd::OBJECTCELLTYPE_COMMON, 0, (WORD)-1, (WORD)-1);
			return true;
		}

	}
	else
		return Package::add(object, find);
}

/**
 * \brief 从包裹中删除物品
 * \param object  物品对象
 */
bool MainPack::remove(zObject *object)
{
	if(object && object->base->kind==ItemType_Money)
	{
		if(gold==object) gold=NULL;
		return true;
	}
	else
		return Package::remove(object);
}

/**
 * \brief 检查并添加
 * \param pUser 角色
 * \param object 物品
 * \param x,y 坐标
 * \return true 有该物品，false 没有
 */
bool MainPack::checkAdd(SceneUser *pUser,zObject *object,WORD x,WORD y)
{
	if(object && object->base->kind==ItemType_Money)
		return (gold==NULL);
	else
		return  Package::checkAdd(pUser,object,x,y);
}

/**
 * \brief 根据物品位置和大小获取物品
 * \param ret 返回找到的物品对象
 * \param x 横坐标
 * \param y 纵坐标
 * \param width 宽度
 * \param height 高度
 * \return true 有此物品 false 无此物品
 */
bool MainPack::getObjectByZone(zObject **ret,WORD x,WORD y)
{
	if(x==(WORD)-1 && y==(WORD)-1)
	{
		*ret=gold;
		return true;
	}
	else
		return  Package::getObjectByZone(ret,x,y);
}

/**
 * \brief 获得金子数目
 * \return 金子树目
 */
DWORD MainPack::getGoldNum()
{
	if(gold)
	{
		return gold->data.dwNum;
	}
	else
	{
		return 0;
	}
}

/**
 * \brief 获得金子对象
 * \return  物品对象或NULL
 */
zObject * MainPack::getGold()
{
	return gold;
}

/**
 * \brief 箭支消耗数量是以耐久度计算的,当耐久为0时删除箭桶
 * \param pThis 主人
 * \param id 物品的objectid
 * \param num　技能消耗物品的数量
 * \author whj 
 * \return 消耗是否成功
 */
bool MainPack::skillReduceObject(SceneUser* pThis, DWORD id, DWORD num)
{
/*
	std::set<zObject *>::iterator it = allset.begin();
	while (it != allset.end()) 
*/
	for (int i=0; i<16; ++i)
	{
		if (!container[i]) continue;
		
		if ( container[i]->data.dwObjectID == id) 
		{
			if((short)container[i]->data.dur <= 1)
			{
/*
				pThis->packs.rmObject(*it);
				Cmd::stRemoveObjectPropertyUserCmd rm;
				rm.qwThisID=(*it)->data.qwThisID;
				pThis->sendCmdToMe(&rm,sizeof(rm));
*/
				pThis->packs.removeObject(container[i], true, false/*?*/);
				return true;
			}
			else
			{
				container[i]->data.dur -=1;
				Cmd::stDurabilityUserCmd std;
				std.dwThisID = container[i]->data.qwThisID;
				std.dwDur = container[i]->data.dur;
				std.dwMaxDur = container[i]->data.maxdur;
				pThis->sendCmdToMe(&std , sizeof(std));
				//Zebra::logger->debug("消耗箭支id=%u,thisid=%u,dur=%u",id ,std.dwThisID , std.dwDur);
				return true;
			}
			break;
		}
		///++it;		
	}

	return false;
}


/**
 * \brief 仓库构造函数，初始化仓库类型并定义仓库大小
 */
StorePack::StorePack() : Package(Cmd::OBJECTCELLTYPE_STORE, 0,  W, H*3)
{

}

/**
 * \brief 仓库构造函数（空）
 */
StorePack::~StorePack()
{


}

#include "NpcTrade.h"

/**
 * \brief 检查指定位置上是否有指定的对象
 * \param pUser 仓库拥有者
 * \param object 物品对象
 * \param x 横坐标
 * \param y 纵坐标
 * \return true 在指定位置上有指定对象 false 检查失败
 */
bool StorePack::checkAdd(SceneUser * pUser, zObject * object, WORD x, WORD y)
{
	if (!object) return true;

	//check if npc can do this?
	NpcTrade::NpcItem item;
	item.id = object->data.dwObjectID;
	item.kind = object->base->kind;
	item.lowLevel = 0;
	item.level = object->data.needlevel;
	item.action = NpcTrade::NPC_STORE_OBJECT;
	if ( !NpcTrade::getInstance().verifyNpcAction(pUser->npc_dwNpcDataID, item) ) {
		return false;
	}

	//check if npc near by user?
	//醉梦 随身仓库
	if(pUser->npc_dwNpcTempID != 100000002)
	{
		SceneNpc * npc = SceneNpcManager::getMe().getNpcByTempID(pUser->npc_dwNpcTempID);
		if (!npc  || !npc->scene || npc->scene != pUser->scene 
				|| !pUser->scene->zPosShortRange(pUser->getPos(), npc->getPos(), SCREEN_WIDTH , SCREEN_HEIGHT)
			) 	{
			return false;
		}
	}
	

	BYTE page = y / H;
	if (page < days.size() && days[page]) {
		return Package::checkAdd(pUser, object, x, y);
	}
	return false;
}
void StorePack::goldstore(SceneUser *pThis,bool notify)
{
	//如果已经有两个包裹并且是vip用户则赠送第三个
	if(pThis->packs.store.days.size()<=2 && (pThis->charbase.bitmask & CHARBASE_VIP))
	{
		pThis->packs.store.days.push_back(1);
		if(notify)
		{
			pThis->packs.store.notify(pThis);
			Channel::sendSys(pThis, Cmd::INFO_TYPE_GAME, "恭喜,你获得系统赠送的一个仓库"); 
		}
	}
}

/**
 * \brief 仓库加载
 * \param dest 目标数据
 * \return 包大小
 */
int StorePack::load(unsigned char* dest)
{
	BYTE size = *(BYTE *)dest;
	dest += sizeof(BYTE);
	
	if (size > 0) {
		days.resize(size);
		memcpy(&days[0], dest, sizeof(BYTE)*size);
	}
	
	return sizeof(BYTE) + sizeof(BYTE)*size;
}

/**
 * \brief 仓库存储
 * \param dest 目标数据
 * \return 包大小
 */
int StorePack::save(unsigned char* dest)
{
	BYTE count = days.size();
	memcpy(dest, &count, sizeof(count));
	dest += sizeof(count);
	
	memcpy(dest, &days[0], sizeof(BYTE)*count);
	
	return sizeof(count) + sizeof(BYTE)*count;
}

/**
 * \brief 发送仓库更新
 * \param user 角色
 */
void StorePack::notify(SceneUser* user)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stStoreInfoNpcTradeUserCmd* info = (Cmd::stStoreInfoNpcTradeUserCmd*)buf;
	constructInPlace(info);
	info->page = days.size();
	memcpy(&info->day[0], &days[0], days.size());
	
	user->sendCmdToMe(info, sizeof(Cmd::stStoreInfoNpcTradeUserCmd) + sizeof(BYTE)*days.size());
}

/**
 * \brief 能否换色
 * \return true 能 false 否
 */
bool EquipPack::canChangeColor()
{
	/// 王霆要求,目的是为了方便通过颜色字段做装备特效
	/*
	if(equip(BODY) && (equip(BODY)->base->color == 0XFFFFFFFF))
	{
		return true;
	}
	else
	{
		return false;
	}
	// */
	return true;
}

/**
 * \brief 构造函数
 */
Packages::Packages(SceneUser* user) : owner(user), equip(user)
{

}

/**
 * \brief  析构函数
 */
Packages::~Packages()
{

}

/**
 * \brief 根据类型获取包裹
 * \param type 包裹类型
 * \param id 目前未使用
 * \return 包裹对象
 */
Package * Packages::getPackage(DWORD type,DWORD id)
{
	switch(type)
	{
		case Cmd::OBJECTCELLTYPE_COMMON:
			return (Package *)&main;
		case Cmd::OBJECTCELLTYPE_EQUIP:
			return (Package *)&equip;
		case Cmd::OBJECTCELLTYPE_STORE:
			return (Package *)&store;
		case Cmd::OBJECTCELLTYPE_PACKAGE:
			if (equip.pack(EquipPack::L_PACK) && equip.pack(EquipPack::L_PACK)->object()->data.qwThisID == id) {
				return (Package *)equip.pack(EquipPack::L_PACK);
			}
			if (equip.pack(EquipPack::R_PACK) && equip.pack(EquipPack::R_PACK)->object()->data.qwThisID == id) {
				return (Package *)equip.pack(EquipPack::R_PACK);
			}
		case Cmd::OBJECTCELLTYPE_MAKE:
			if (equip.pack(EquipPack::L_MAKE) && equip.pack(EquipPack::L_MAKE)->object()->data.qwThisID == id) {
				return (Package *)equip.pack(EquipPack::L_MAKE);
			}
			if (equip.pack(EquipPack::R_MAKE) && equip.pack(EquipPack::R_MAKE)->object()->data.qwThisID == id) {
				return (Package *)equip.pack(EquipPack::R_MAKE);
			}
			break;
		case Cmd::OBJECTCELLTYPE_PET:
			return (Package *)&petPack;
			break;
		//////////////////////////////////////////////	
        case Cmd::OBJECTCELLTYPE_RECAST:
            return (Package *)&recastPack;
		//////////////////////////////////////////////	
		case Cmd::OBJECTCELLTYPE_ANNEX:
            return (Package *)&annexPack;
		//////////////////////////////////////////////	
		case Cmd::OBJECTCELLTYPE_HANDBOOK:
            return (Package *)&handbookPack;
		//////////////////////////////////////////////
		case Cmd::OBJECTCELLTYPE_FABAO:
            return (Package *)&fabaoPack;
		//////////////////////////////////////////////
		case Cmd::OBJECTCELLTYPE_FUJIAN:
            return (Package *)&fujianPack;
			//////////////////////////////////////////////
		case Cmd::OBJECTCELLTYPE_YUANSHEN:
            return (Package *)&yuanshenPack;
		default:
			break;
	}
	return NULL;
}

/*
void Packages::clearPackage(Package* pack)
{
	ClearPack cp(this);
	pack->execEvery(cp);
}
*/

Package** Packages::getPackage(int packs)
{
//////////////////////////////////////////////////////////
    const DWORD PACK_MAX_COUNT = 9;
	Package** p = new Package*[PACK_MAX_COUNT];
	memset(p, 0, PACK_MAX_COUNT*sizeof(Package *));
//////////////////////////////////////////////////////////	

	//notice the sequence
	int i = 0;
	if (packs & MAIN_PACK) p[i++] = (Package *)&main;
	if ((packs & LEFT_PACK) && equip.pack(EquipPack::L_PACK) ) p[i++] =  (Package *)equip.pack(EquipPack::L_PACK);	
	if ((packs & RIGHT_PACK) && equip.pack(EquipPack::R_PACK) ) p[i++] = (Package *)equip.pack(EquipPack::R_PACK);	
	
	if (packs & EQUIP_PACK) p[i++] = (Package *)&equip;
	if (packs & STORE_PACK) p[i++] = (Package *)&store;
	
	if ((packs & LM_PACK) && equip.pack(EquipPack::L_MAKE) ) p[i++] = (Package *)equip.pack(EquipPack::L_MAKE);	
	if ((packs & RM_PACK) && equip.pack(EquipPack::R_MAKE) ) p[i++] = (Package *)equip.pack(EquipPack::R_MAKE);

	if (packs & PET_PACK) p[i++] = (Package *)&petPack;
	////////////////////////////////////////////////////////
	if (packs & RECAST_PACK) p[i++] = (Package *)&recastPack;
	////////////////////////////////////////////////////////
	if (packs & RECAST_PACK) p[i++] = (Package *)&annexPack;
	////////////////////////////////////////////////////////	
	if (packs & RECAST_PACK) p[i++] = (Package *)&handbookPack;
	////////////////////////////////////////////////////////	

	if (packs & RECAST_PACK) p[i++] = (Package *)&fabaoPack;
	////////////////////////////////////////////////////////	
	if (packs & RECAST_PACK) p[i++] = (Package *)&fujianPack;
	////////////////////////////////////////////////////////	
	if (packs & RECAST_PACK) p[i++] = (Package *)&yuanshenPack;


	return p;
}

/**
 * \brief 获得当前金子数量
 * \return 金子数量
 */
DWORD Packages::getGoldNum()
{
	return main.getGoldNum();
}

/**
 * \brief 获取身上的金子
 * \return 物品对象，或NULL
 */
zObject *Packages::getGold()
{
	return main.getGold();
}

/**
 * \brief 将物品丢到地上
 * \param o 目标物品 
 * \param pos 位置
 * \return true 无聊的返回值
 */
bool Packages::moveObjectToScene(zObject *o , const zPos &pos , DWORD overdue_msecs, const unsigned long dwID)
{
	removeObject(o, true, false); //notify but not delete
	zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,o->data.dwNum,0,owner->id,owner->name,owner->scene->id,owner->scene->name,"掉落",o->base,o->data.kind,o->data.upgrade);
	//到地上
	if(!owner->scene->addObject(o , pos , overdue_msecs , dwID))
	{
		Zebra::logger->trace("%s(%ld)凋落的装备添加倒场景失败",
				owner->name,owner->id);
		//zObject::destroy(o);
	}
	return true;
}

/**
 * \brief 移动物品
 * \param pUser 角色对象
 * \param srcObj 被移动对象
 * \param dst 对象的目的位置
 * \return true 移动成功， false 移动失败
 */
bool Packages::moveObject(SceneUser *pUser,zObject *srcObj,stObjectLocation &dst)
{
	using namespace Cmd;

	Package *srcpack = getPackage(srcObj->data.pos.loc(), srcObj->data.pos.tab());
	if (!srcpack)  {
		Zebra::logger->warn("物品%s[%x]定位错误，不能移动", srcObj->name, srcObj);		
		return false;
	}
	if(srcpack->type() == Cmd::OBJECTCELLTYPE_EQUIP)
	{
		if (
				(equip.pack(EquipPack::L_PACK) && 
				 equip.pack(EquipPack::L_PACK)->object()->data.qwThisID == srcObj->data.qwThisID && 
				 !equip.pack(EquipPack::L_PACK)->empty()) ||
				(equip.pack(EquipPack::R_PACK) && 
				 equip.pack(EquipPack::R_PACK)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::R_PACK)->empty()) ||
				(equip.pack(EquipPack::L_MAKE) && 
				 equip.pack(EquipPack::L_MAKE)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::L_MAKE)->empty()) ||
				(equip.pack(EquipPack::R_MAKE) && 
				 equip.pack(EquipPack::R_MAKE)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::R_MAKE)->empty())
		   )
		{
			Zebra::logger->warn("不能移动装有物品的包裹%s(%u),%s(%u)",pUser->name,pUser->id,srcObj->data.strName,srcObj->data.qwThisID);
			return false;
		}
	}


	if (dst.loc() == Cmd::OBJECTCELLTYPE_NONE) {
		zObject::logger(srcObj->createid,srcObj->data.qwThisID,srcObj->base->name,srcObj->data.dwNum,srcObj->data.dwNum,0,0,NULL,pUser->id,pUser->name,"扔东西",srcObj->base,srcObj->data.kind,srcObj->data.upgrade);
		removeObject(srcObj);
		return true;
	}

	Package *destpack = getPackage(dst.loc(), dst.tab());
	if(!destpack) return false;


	if(srcpack->type() == Cmd::OBJECTCELLTYPE_STORE && destpack->type() != Cmd::OBJECTCELLTYPE_STORE)
	{
		if (srcObj->data.pos.ypos()>=6 && pUser->isSafety(Cmd::SAFE_THIRD_PACK))
		{// 仓库中,第六行以后,就是第二.三仓库
			
			return false;
		}
	}

	if(destpack->type() == Cmd::OBJECTCELLTYPE_EQUIP)
	{
		if (
				(equip.pack(EquipPack::L_PACK) && 
				 equip.pack(EquipPack::L_PACK)->object()->data.qwThisID == srcObj->data.qwThisID && 
				 !equip.pack(EquipPack::L_PACK)->empty()) ||
				(equip.pack(EquipPack::R_PACK) && 
				 equip.pack(EquipPack::R_PACK)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::R_PACK)->empty()) ||
				(equip.pack(EquipPack::L_MAKE) && 
				 equip.pack(EquipPack::L_MAKE)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::L_MAKE)->empty()) ||
				(equip.pack(EquipPack::R_MAKE) && 
				 equip.pack(EquipPack::R_MAKE)->object()->data.qwThisID == srcObj->data.qwThisID &&
				 !equip.pack(EquipPack::R_MAKE)->empty())
		   )
		{
			Zebra::logger->warn("不能移动装有物品的包裹%s(%u),%s(%u)",pUser->name,pUser->id,srcObj->data.strName,srcObj->data.qwThisID);
			return false;
		}
		//soke 检查ID
		OnUse event(srcObj->base->id);
		EventTable::instance().execute(*pUser, event);
		//////////////////////////////////////
	}


	zObject *destObj = NULL;
	if(destpack->getObjectByZone(&destObj,dst.xpos(),dst.ypos()) && destObj != srcObj)
	{

		if(destpack->checkAdd(pUser,srcObj,dst.xpos(),dst.ypos()))
		{

			if(srcpack->checkAdd(pUser,destObj,srcObj->data.pos.xpos(),srcObj->data.pos.ypos()))
			{

				//宠物包裹只能拿出不能放入
				if (dst.loc()==Cmd::OBJECTCELLTYPE_PET
						|| (srcObj->data.pos.loc()==Cmd::OBJECTCELLTYPE_PET && destObj))
					return false;

				destpack->remove(destObj);
				if(destObj) destObj->data.pos=srcObj->data.pos;

				srcpack->remove(srcObj);
				srcpack->add(destObj,false);

				srcObj->data.pos=dst;
				destpack->add(srcObj,false);

				if(dst.loc() ==Cmd::OBJECTCELLTYPE_EQUIP)//装备时消耗耐久
				{
					switch(srcObj->base->kind)
					{

						case ItemType_DoubleExp:
							{
								if(srcObj->data.dur> 60)
								{
									if(!pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN , ItemType_DoubleExp , 60 , true , true))
									{
										pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_DoubleExp , 60 , true , true);
									}
								}
								/*
								   else
								   {
								   Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "耐久度太低,不能装备"); 
								   return false;
								   }
								// */
							}
							break;
						case ItemType_Tonic:
							{
								if(srcObj->data.dur> 60)
								{
									if(!pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN , ItemType_Tonic , 60 , true , false))
									{
										pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_Tonic , 60 , true , false);
									}
								}
								/*
								   else
								   {
								   Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "耐久度太低,不能装备"); 
								   return false;
								   }
								// */
							}
							break;
						case ItemType_Amulet:
							{
								if(srcObj->data.dur> 60)
								{
									if(!pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN , ItemType_Amulet , 60 , true , false))
									{
									pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_ADORN + 1 , ItemType_Amulet , 60 , true , false);
									}
								}
								/*
								   else
								   {
								   Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "耐久度太低,不能装备"); 
								   return false;
								   }
								// */
							}
							break;
						case ItemType_FashionBody: //soke 低级时装 119
							{
								pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_FASHION, ItemType_FashionBody,1 , true ,false);
							}
							break;
						case ItemType_HighFashionBody: //soke 高级时装 124
							{
								pUser->packs.equip.reduceDur(pUser , Cmd::EQUIPCELLTYPE_FASHION, ItemType_HighFashionBody,1 , true ,false);
							}
							break;
                        ////////////////////////////////////////////////////////////
						case ItemType_GreatLeechdomMp:
							{
								//检查是否有自动补魔道具
								pUser->checkAutoMP();
							}
							break;
						case ItemType_GreatLeechdomHp:
							{
								//soke 检查是否有自动补血道具
								pUser->checkAutoHP();
							}
							break;
						default:
							break;
					}
				}

				return true;
			}
		}
	}
	else
	{
		if(destObj == srcObj )
		{
			Zebra::logger->debug("自己移动到自己的位置bug");
		}

	}

	return false;
}

/**
 * \brief	删除包裹中的对象
 * \param   srcObj 目标物品
 */
bool Packages::removeObject(zObject *srcObj, bool notify, bool del)
{
	if(!srcObj) return false;

	if (notify) {
		Cmd::stRemoveObjectPropertyUserCmd send;
		send.qwThisID=srcObj->data.qwThisID;
		owner->sendCmdToMe(&send,sizeof(send));
	}

	uom.removeObject(srcObj);
	Package *p=getPackage(srcObj->data.pos.loc(),srcObj->data.pos.tab());
	if(p && p->remove(srcObj)) {
		//only delete when remove from package and del is true
		if (del) zObject::destroy(srcObj);
		return true;
	}

	Zebra::logger->warn("物品%s[%x]定位错误，不能删除", srcObj->name, srcObj);		
	return false;
}

#include "QuestEvent.h"
#include "QuestTable.h"

/**
 * \brief 增加物品
 * \param srcObj 物品对象
 * \param needFind 是否要查找位置
 * \param from_record 是否来自记录
 * \return false 添加失败 true 添加成功
 */
bool Packages::addObject(zObject *srcObj, bool needFind, int packs)
{
	if(srcObj)
	{
        switch (srcObj->base->kind)
        {
                    case ItemType_Blade:		    //104代表武术刀类武器
					case ItemType_Sword :	        //105代表武术剑类武器
					case ItemType_Axe:	            //106代表武术斧类武器
					case ItemType_Hammer:	        //107代表武术斧类武器
					case ItemType_Staff:		    //108代表法术杖类武器
					case ItemType_Crossbow:	        //109代表箭术弓类武器
					case ItemType_Fan:	            //110代表美女扇类
					case ItemType_Stick:	        //111代表召唤棍类武器			
					case ItemType_Necklace:	        //117代表角色项链类
					case ItemType_Fing:		        //118代表角色戒指类
					case ItemType_ClothBody:		//101代表布质加生命类服装
					case ItemType_FellBody:		    //102代表皮甲加魔防类服装
					case ItemType_MetalBody:		//103代表金属铠甲加物防类服装
					case ItemType_Shield:	        //112代表盾牌类
					case ItemType_Helm:		        //113代表角色头盔类
					case ItemType_Caestus:	        //114代表角色腰带类
					case ItemType_Cuff:		        //115代表角色护腕类
					case ItemType_Shoes:		    //116代表角色鞋子类
					case ItemType_HorseShoe:        //130代表坐骑马脚类
			        case ItemType_HorseRope:        //131代表坐骑马绳类
			        case ItemType_HorseSaddle:      //132代表坐骑马鞍类
			        case ItemType_HorseSafe:        //133代表坐骑马甲类
			        case ItemType_HorseIron:        //134代表坐骑马镫类
					case ItemType_Dagger:	        //136代表利刃
					case ItemType_NewSword:	        //137代表卫士剑
	        		case ItemType_breastplate:	    //138代表护心镜
	        		case ItemType_StaffFlag:		//141代表战旗(法师)
	        		case ItemType_CrossFlag:	    //142代表战旗(弓手)
	        		case ItemType_Flag:	            //143代表战旗(仙术)
	        		case ItemType_StickFlag:	    //144代表战旗(召唤)
	        		case ItemType_BladeFlag:	    //145代表战旗(战士)
	        		case ItemType_DaggerFlag:	    //146代表战旗(刺客)
	        		case ItemType_SwordFlag:        //147代表战旗(卫士)
	        		case ItemType_LongGun:          //155代表长枪
	        		case ItemType_Handgun:          //156代表短枪
            break;
        default:
            srcObj->data.exp = 0;
            break;
        }

        assert( !needFind || srcObj->free() ); //be sure a object do not be added into 2 diff package
        if(uom.addObject(srcObj))	
        {
			if (!packs) 
            {
				Package *p = getPackage(srcObj->data.pos.loc(),srcObj->data.pos.tab());
				if(p && p->add(srcObj, needFind)) return true;
			}

			if (packs) 
            {
				//save location infomation
				stObjectLocation loc = srcObj->reserve();

				Package** p = getPackage(packs);
				int i = 0;
				while (p && p[i]) 
                {
					srcObj->data.pos = stObjectLocation(p[i]->type(), p[i]->id(), (WORD)-1, (WORD)-1);
					if (p[i]->add(srcObj, needFind)) 
                    {
						SAFE_DELETE_VEC(p);
						return true;
					}
					++i;
				}
				SAFE_DELETE_VEC(p);

				//can not be add, resotre the location
				srcObj->restore(loc);
			}
			//Zebra::logger->warn("物品%s[%x]定位错误，不能添加", srcObj->name, srcObj);
			uom.removeObject(srcObj);
			
			return false;
		}
        else 
        {
			//Zebra::logger->warn("物品%s[%x]索引重复，不能添加", srcObj->name, srcObj);
		}
	}
	return false;
}

/**     
 * \brief 包裹积分验证
 *      
 * 验证用户包裹中的是否满足要求
 
 * \param need: 需要积分
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkTicket(DWORD need)
{
	if(owner->charbase.ticket >= need)
	{
		return true;
	}
	
	return false;	
}

/**     
 * \brief 包裹工资验证
 *      
 * 验证用户包裹中的工资是否满足要求
 
 * \param need: 需要工资
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkSalary(DWORD need)
{
	if(owner->charbase.salary >= need)
	{
		return true;
	}
	
	return false;	
}

/**     
 * \brief 包裹VIP点数验证
 *      
 * 验证用户包裹中的VIP点数是否满足要求
 
 * \param need: 需要VIP点数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkCharvip(DWORD need)
{
	if(owner->charbase.charvip >= need)
	{
		return true;
	}
	
	return false;	
}
/**     
 * \brief 包裹充值点数验证
 *      
 * 验证用户包裹中的充值点数是否满足要求
 
 * \param need: 需要充值点数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkChongzhi(DWORD need)
{
	if(owner->charbase.chongzhi >= need)
	{
		return true;
	}
	
	return false;	
}
/**     
 * \brief 包裹职业点数验证
 *      
 * 验证用户包裹中的职业点数是否满足要求
 
 * \param need: 需要职业点数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkChangejob(DWORD need)
{
	if(owner->charbase.changejob >= need)
	{
		return true;
	}
	
	return false;	
}

/**     
 * \brief 包裹鲜花验证
 *      
 * 验证用户包裹中的鲜花数是否满足要求
 
 * \param need: 需要鲜花数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkFolwers(DWORD need)
{
	if(owner->charbase.folwers >= need)
	{
		return true;
	}
	
	return false;	
}

/**     
 * \brief 包裹人气验证
 *      
 * 验证用户包裹中的人气数是否满足要求
 
 * \param need: 需要人气数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkMoods(DWORD need)
{
	if(owner->charbase.moods >= need)
	{
		return true;
	}
	
	return false;	
}

/**     
 * \brief 包裹爱心验证
 *      
 * 验证用户包裹中的爱心数是否满足要求
 
 * \param need: 需要爱心数
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkFlloves(DWORD need)
{
	if(owner->charbase.flloves >= need)
	{
		return true;
	}
	
	return false;	
}

bool Packages::checkAngel(DWORD need)
{
	if(owner->charbase.angel >= need)
	{
		return true;
	}

	return false;	
}

bool Packages::checkHanbing(DWORD need)
{
	if(owner->charbase.hanbing >= need)
	{
		return true;
	}

	return false;	
}


/**     
 * \brief 包裹金子验证
 *      
 * 验证用户包裹中的金子是否满足要求
 
 * \param need: 需要金子
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkGold(DWORD need)
{
	if(owner->charbase.gold >= need)
	{
		return true;
	}
	
	return false;	
}
/**     
 * \brief 包裹银子验证
 *      
 * 验证用户包裹中的银子是否满足要求
 
 * \param need: 需要银子
 * \return 验证通过返回true,否则返回false
 */
bool Packages::checkMoney(DWORD need)
{
	if (!need) return true;
	
	zObject* gold = getGold();
	//temp solution, just for record before
	if (!gold) {
		gold = zObject::create(objectbm.get(665), 0);
		if (gold) {
			owner->packs.addObject(gold, true, MAIN_PACK);
		}else {
			Zebra::logger->error("创建银子失败");
		}

	}

	if (!gold) return false;
	if ( gold->data.dwNum < need ) {
		//Channel::sendSys(&user, Cmd::INFO_TYPE_FAIL, "银子不足"); 
		return false;
	}
	
	return true;	
}

/**     
 * \brief 扣除银子
 *      
 * 从用户包裹扣除银子,该函数不会检查扣除的金额,注意防止溢出
 
 * \param num: 扣除的银子数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeMoney(DWORD num ,const char *disc)
{	
#ifdef _DEBUGLOG
	Zebra::logger->alarm("用户(%s)银子扣除操作进入，期望(%d)", owner->name, num);
#endif
	if (!num) return true;
	
	zObject* gold = getGold();
	//temp solution, just for record before
	if (!gold) {
		gold = zObject::create(objectbm.get(665), 0);
		if (gold) {
			owner->packs.addObject(gold, true, MAIN_PACK);
		}else {
			Zebra::logger->error("创建银子失败");
		}
	}

	if (!gold) return false;

#ifdef _DEBUGLOG
	Zebra::logger->alarm("用户(%s)银子扣除操作，期望(%d),现有(%d)", owner->name, num, gold->data.dwNum);
#endif
	
	if (gold->data.dwNum < num) {
		Zebra::logger->alarm("用户(%s)银子扣除失败，期望(%d),现有(%d)", owner->name, num, gold->data.dwNum);
		return false;
	}
	
	gold->data.dwNum -= num;
	zObject::logger(owner->charbase.accid,gold->data.qwThisID,gold->data.strName,gold->data.dwNum,num,0,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	
	Cmd::stRefCountObjectPropertyUserCmd ret;
	ret.qwThisID = gold->data.qwThisID;
	ret.dwNum = gold->data.dwNum;
	owner->sendCmdToMe(&ret, sizeof(ret));
	
	return true;
}

/**     
 * \brief 扣除积分
 *      
 
 * \param num: 扣除的积分数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeTicket(DWORD num ,const char *disc)
{
	if(owner->charbase.ticket < num)
	{
		Zebra::logger->debug("%s(%d)积分不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.ticket,disc);
		return false;
	}
	owner->charbase.ticket-=num;
	zObject::logger(0,0,"积分",owner->charbase.ticket,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}


/**
 * \brief 添加积分
 * \param num 积分数量
 * \param note 提示信息
 */
void Packages::addTicket(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	//myy 积分上限21Y点
	DWORD Ticketx = 2100000000;
	if ((owner->charbase.ticket + num) > Ticketx)
	{
		owner->charbase.ticket+= (Ticketx - owner->charbase.ticket);
		zObject::logger(owner->charbase.accid,owner->charbase.level,"积分",owner->charbase.ticket,(Ticketx - owner->charbase.ticket),1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , (Ticketx - owner->charbase.ticket), "你得到积分");
			}       
			else                    
			{       
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , (Ticketx - owner->charbase.ticket), disc);
			}
		}
	}
	else
	{
		owner->charbase.ticket+=num;
		zObject::logger(owner->charbase.accid,owner->charbase.level,"积分",owner->charbase.ticket,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到积分%d",num);
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , num, "你得到积分");
			}       
			else                    
			{       
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , num, disc);
			}
		}
	}

}	

/**
 * \brief 兑换积分
 * \param num 积分数量
 * \param note 提示信息
 */
void Packages::addTicket2(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	//myy 积分上限21Y点
	DWORD Ticketx = 2100000000;
	if ((owner->charbase.ticket + num) > Ticketx)
	{
		owner->charbase.ticket+= (Ticketx - owner->charbase.ticket);
		zObject::logger(owner->charbase.accid,owner->charbase.level,"积分",owner->charbase.ticket,(Ticketx - owner->charbase.ticket),1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , (Ticketx - owner->charbase.ticket), "兑换得到积分");
			}       
			else                    
			{       
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , (Ticketx - owner->charbase.ticket), disc);
			}
		}
	}
	else
	{
		owner->charbase.ticket+=num;
		zObject::logger(owner->charbase.accid,owner->charbase.level,"积分",owner->charbase.ticket,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到积分%d",num);
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , num, "兑换得到积分");
			}       
			else                    
			{       
				Channel::sendTicket(owner, Cmd::INFO_TYPE_GAME , num, disc);
			}
		}
	}

}
/**     
 * \brief 扣除充值点
 *      
 
 * \param num: 扣除的充值点数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeChongzhi(DWORD num ,const char *disc)
{
	if(owner->charbase.chongzhi < num)
	{
		Zebra::logger->debug("%s(%d)充值点不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.chongzhi,disc);
		return false;
	}
	owner->charbase.chongzhi-=num;
	zObject::logger(0,0,"充值点",owner->charbase.chongzhi,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}


/**
 * \brief 添加充值点
 * \param num 充值点数量
 * \param note 提示信息
 */
void Packages::addChongzhi(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	//myy 充值点上限21Y点
	DWORD Chongzhix = 2100000000;
	if ((owner->charbase.chongzhi + num) > Chongzhix)
	{
		owner->charbase.chongzhi+= (Chongzhix - owner->charbase.chongzhi);
		zObject::logger(owner->charbase.accid,owner->charbase.level,"充值点",owner->charbase.chongzhi,(Chongzhix - owner->charbase.chongzhi),1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				Channel::sendChongzhi(owner, Cmd::INFO_TYPE_GAME , (Chongzhix - owner->charbase.chongzhi), "你得到充值点");
			}       
			else                    
			{       
				Channel::sendChongzhi(owner, Cmd::INFO_TYPE_GAME , (Chongzhix - owner->charbase.chongzhi), disc);
			}
		}
	}
	else
	{
		owner->charbase.chongzhi+=num;
		zObject::logger(owner->charbase.accid,owner->charbase.level,"充值点",owner->charbase.chongzhi,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到充值点%d",num);
				Channel::sendChongzhi(owner, Cmd::INFO_TYPE_GAME , num, "你得到充值点");
			}       
			else                    
			{       
				Channel::sendChongzhi(owner, Cmd::INFO_TYPE_GAME , num, disc);
			}
		}
	}

}
/**     
 * \brief 扣除宝石数
 *      
 
 * \param num: 扣除的宝石数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeGoldgive(DWORD num ,const char *disc)
{
	if(owner->charbase.goldgive < num)
	{
		Zebra::logger->debug("%s(%d)宝石数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.goldgive,disc);
		return false;
	}
	owner->charbase.goldgive-=num;
	zObject::logger(0,0,"宝石数",owner->charbase.goldgive,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}

/**     
 * \brief 扣除工资
 *      
 
 * \param num: 扣除的工资数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeSalary(DWORD num ,const char *disc)
{
	if(owner->charbase.salary < num)
	{
		Zebra::logger->debug("%s(%d)工资不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.salary,disc);
		return false;
	}
	owner->charbase.salary-=num;
	zObject::logger(0,0,"工资",owner->charbase.salary,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}
/**
 * \brief 添加工资
 * \param num 工资数量
 * \param note 提示信息
 */
void Packages::addSalary(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.salary+=num;
	zObject::logger(0,0,"工资",owner->charbase.salary,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到工资%d",num);
			Channel::sendSalary(owner, Cmd::INFO_TYPE_GAME , num, "你得到工资");
		}       
		else                    
		{       
			Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "%s消费使你得到工资%d",note,num);
		}
	}
}

/**
 * \brief 添加领取工资
 * \param num 工资数量
 * \param note 提示信息
 */
void Packages::addSalary2(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.salary+=num;
	zObject::logger(0,0,"工资",owner->charbase.salary,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到工资%d",num);
			Channel::sendSalary(owner, Cmd::INFO_TYPE_GAME , num, "恭喜您领到了工资");
		}       
	}
}

/**
 * \brief 挖宝添加工资
 * \param num 挖宝工资数量
 * \param note 提示信息
 */
void Packages::addSalary3(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.salary+=num;
	zObject::logger(0,0,"工资",owner->charbase.salary,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendSalary(owner, Cmd::INFO_TYPE_GAME , num, "挖到工资");
		}       
	}
}

/**     
 * \brief 扣除VIP点数
 *      
 
 * \param num: 扣除的VIP点数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeCharvip(DWORD num ,const char *disc)
{
	if(owner->charbase.charvip < num)
	{
		Zebra::logger->debug("%s(%d)VIP点数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.charvip,disc);
		return false;
	}
	owner->charbase.charvip-=num;
	zObject::logger(0,0,"VIP点数",owner->charbase.charvip,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));

	if(owner->charbase.charvip > 2690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP20);
		owner->showCurrentEffect(Cmd::USTATE_VIP19, false);
	}
	else if(owner->charbase.charvip > 2190 && owner->charbase.charvip<=2690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP19);
		owner->showCurrentEffect(Cmd::USTATE_VIP18, false);
	}
	else if(owner->charbase.charvip > 1690 && owner->charbase.charvip<=2190) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP18);
		owner->showCurrentEffect(Cmd::USTATE_VIP17, false);
	}
	else if(owner->charbase.charvip > 1190 && owner->charbase.charvip<=1690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP17);
		owner->showCurrentEffect(Cmd::USTATE_VIP16, false);
	}
  	else if(owner->charbase.charvip > 690 && owner->charbase.charvip<=1190) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP16);
		owner->showCurrentEffect(Cmd::USTATE_VIP15, false);
	}
	else if(owner->charbase.charvip > 190 && owner->charbase.charvip<=690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP15);
		owner->showCurrentEffect(Cmd::USTATE_VIP14, false);
	}
	else if(owner->charbase.charvip > 170 && owner->charbase.charvip<=190) //soke VIP14玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP14);
		owner->showCurrentEffect(Cmd::USTATE_VIP13, false);
	}
	else if(owner->charbase.charvip > 150 && owner->charbase.charvip<=170) //soke VIP13玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP13);
		owner->showCurrentEffect(Cmd::USTATE_VIP12, false);
	}
	else if(owner->charbase.charvip > 130 && owner->charbase.charvip<=150) //soke VIP12玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP12);
		owner->showCurrentEffect(Cmd::USTATE_VIP11, false);
	}
	else if(owner->charbase.charvip > 110 && owner->charbase.charvip<=130) //soke VIP11玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP11);
		owner->showCurrentEffect(Cmd::USTATE_VIP10, false);
	}
	else if(owner->charbase.charvip > 90 && owner->charbase.charvip<=110) //soke VIP10玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP10);
		owner->showCurrentEffect(Cmd::USTATE_VIP9, false);
	}
	else if(owner->charbase.charvip > 80 && owner->charbase.charvip<=90) //soke VIP9玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP9);
		owner->showCurrentEffect(Cmd::USTATE_VIP8, false);
	}
	else if(owner->charbase.charvip > 70 && owner->charbase.charvip<=80) //soke VIP8玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP8);
		owner->showCurrentEffect(Cmd::USTATE_VIP7, false);
	}
	else if(owner->charbase.charvip > 60 && owner->charbase.charvip<=70) //soke VIP7玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP7);
		owner->showCurrentEffect(Cmd::USTATE_VIP6, false);
	}
	else if(owner->charbase.charvip > 50 && owner->charbase.charvip<=60) //soke VIP6玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP6);
		owner->showCurrentEffect(Cmd::USTATE_VIP5, false);
	}
	else if(owner->charbase.charvip > 40 && owner->charbase.charvip<=50) //soke VIP5玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP5);
		owner->showCurrentEffect(Cmd::USTATE_VIP4, false);
	}
	else if(owner->charbase.charvip > 30 && owner->charbase.charvip<=40) //soke VIP4玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP4);
		owner->showCurrentEffect(Cmd::USTATE_VIP3, false);
	}
	else if(owner->charbase.charvip > 20 && owner->charbase.charvip<=30) //soke VIP3玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP3);
		owner->showCurrentEffect(Cmd::USTATE_VIP2, false);
	}
	else if(owner->charbase.charvip > 10 && owner->charbase.charvip<=20) //soke VIP2玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP2);
		owner->showCurrentEffect(Cmd::USTATE_VIP1, false);
	}
	else if(owner->charbase.charvip > 0 && owner->charbase.charvip<=10) //soke VIP1玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP1);
	}
	owner->sendtoSelectedVIPState();
	
	return true;
}
/**
 * \brief 添加VIP点数
 * \param num VIP点数数量
 * \param note 提示信息
 */
void Packages::addCharvip(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.charvip+=num;
	zObject::logger(0,0,"VIP点数",owner->charbase.charvip,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到工资%d",num);
			Channel::sendCharvip(owner, Cmd::INFO_TYPE_GAME , num, "你得到VIP点数：");
		}       
		else                    
		{       
			Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "%s消费使你得到VIP点数%d",note,num);
		}
	}

	if(owner->charbase.charvip > 2690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP20);
		owner->showCurrentEffect(Cmd::USTATE_VIP19, false);
	}
	else if(owner->charbase.charvip > 2190 && owner->charbase.charvip<=2690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP19);
		owner->showCurrentEffect(Cmd::USTATE_VIP18, false);
	}
	else if(owner->charbase.charvip > 1690 && owner->charbase.charvip<=2190) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP18);
		owner->showCurrentEffect(Cmd::USTATE_VIP17, false);
	}
	else if(owner->charbase.charvip > 1190 && owner->charbase.charvip<=1690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP17);
		owner->showCurrentEffect(Cmd::USTATE_VIP16, false);
	}
	else if(owner->charbase.charvip > 690 && owner->charbase.charvip<=1190) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP16);
		owner->showCurrentEffect(Cmd::USTATE_VIP15, false);
	}
  	else if(owner->charbase.charvip > 190 && owner->charbase.charvip<=690) //soke VIP15玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP15);
		owner->showCurrentEffect(Cmd::USTATE_VIP14, false);
	}
	else if(owner->charbase.charvip > 170 && owner->charbase.charvip<=190) //soke VIP14玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP14);
		owner->showCurrentEffect(Cmd::USTATE_VIP13, false);
	}
	else if(owner->charbase.charvip > 150 && owner->charbase.charvip<=170) //soke VIP13玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP13);
		owner->showCurrentEffect(Cmd::USTATE_VIP12, false);
	}
	else if(owner->charbase.charvip > 130 && owner->charbase.charvip<=150) //soke VIP12玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP12);
		owner->showCurrentEffect(Cmd::USTATE_VIP11, false);
	}
	else if(owner->charbase.charvip > 110 && owner->charbase.charvip<=130) //soke VIP11玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP11);
		owner->showCurrentEffect(Cmd::USTATE_VIP10, false);
	}
	else if(owner->charbase.charvip > 90 && owner->charbase.charvip<=110) //soke VIP10玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP10);
		owner->showCurrentEffect(Cmd::USTATE_VIP9, false);
	}
	else if(owner->charbase.charvip > 80 && owner->charbase.charvip<=90) //soke VIP9玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP9);
		owner->showCurrentEffect(Cmd::USTATE_VIP8, false);
	}
	else if(owner->charbase.charvip > 70 && owner->charbase.charvip<=80) //soke VIP8玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP8);
		owner->showCurrentEffect(Cmd::USTATE_VIP7, false);
	}
	else if(owner->charbase.charvip > 60 && owner->charbase.charvip<=70) //soke VIP7玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP7);
		owner->showCurrentEffect(Cmd::USTATE_VIP6, false);
	}
	else if(owner->charbase.charvip > 50 && owner->charbase.charvip<=60) //soke VIP6玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP6);
		owner->showCurrentEffect(Cmd::USTATE_VIP5, false);
	}
	else if(owner->charbase.charvip > 40 && owner->charbase.charvip<=50) //soke VIP5玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP5);
		owner->showCurrentEffect(Cmd::USTATE_VIP4, false);
	}
	else if(owner->charbase.charvip > 30 && owner->charbase.charvip<=40) //soke VIP4玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP4);
		owner->showCurrentEffect(Cmd::USTATE_VIP3, false);
	}
	else if(owner->charbase.charvip > 20 && owner->charbase.charvip<=30) //soke VIP3玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP3);
		owner->showCurrentEffect(Cmd::USTATE_VIP2, false);
	}
	else if(owner->charbase.charvip > 10 && owner->charbase.charvip<=20) //soke VIP2玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP2);
		owner->showCurrentEffect(Cmd::USTATE_VIP1, false);
	}
	else if(owner->charbase.charvip > 0 && owner->charbase.charvip<=10) //soke VIP1玩家特效
	{
		owner->setUState(Cmd::USTATE_VIP1);
	}
	owner->sendtoSelectedVIPState();
}

/**     
 * \brief 扣除职业点数
 *      
 
 * \param num: 扣除的职业点数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeChangejob(DWORD num ,const char *disc)
{
	if(owner->charbase.changejob < num)
	{
		Zebra::logger->debug("%s(%d)职业点数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.changejob,disc);
		return false;
	}
	owner->charbase.changejob-=num;
	zObject::logger(0,0,"职业点数",owner->charbase.changejob,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}
/**
 * \brief 添加职业点数
 * \param num 职业点数数量
 * \param note 提示信息
 */
void Packages::addChangejob(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.changejob+=num;
	zObject::logger(0,0,"职业点数",owner->charbase.changejob,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到工资%d",num);
			Channel::sendChangejob(owner, Cmd::INFO_TYPE_GAME , num, "你得到职业点数：");
		}       
		else                    
		{       
			Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "%s消费使你得到职业点数%d",note,num);
		}
	}
}

/**     
 * \brief 扣除鲜花数
 *      
 
 * \param num: 扣除的鲜花数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeFolwers(DWORD num ,const char *disc)
{
	if(owner->charbase.folwers < num)
	{
		Zebra::logger->debug("%s(%d)鲜花数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.folwers,disc);
		return false;
	}
	owner->charbase.folwers-=num;
	zObject::logger(0,0,"鲜花",owner->charbase.folwers,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}

/**
 * \brief 添加鲜花数
 * \param num 鲜花数数量
 * \param note 提示信息
 */
void Packages::addFolwers(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.folwers+=num;
	zObject::logger(0,0,"鲜花",owner->charbase.folwers,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendFolwers(owner, Cmd::INFO_TYPE_GAME , num, "你得鲜花数：");
		}       
	}
}

/**     
 * \brief 扣除人气数
 *      
 
 * \param num: 扣除的人气数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeMoods(DWORD num ,const char *disc)
{
	if(owner->charbase.moods < num)
	{
		Zebra::logger->debug("%s(%d)人气数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.moods,disc);
		return false;
	}
	owner->charbase.moods-=num;
	zObject::logger(0,0,"人气",owner->charbase.moods,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}

/**
 * \brief 添加人气数
 * \param num 人气数数量
 * \param note 提示信息
 */
void Packages::addMoods(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.moods+=num;
	zObject::logger(0,0,"人气",owner->charbase.moods,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendMoods(owner, Cmd::INFO_TYPE_GAME , num, "你得人气数：");
		}       
	}
}

/**     
 * \brief 扣除爱心数
 *      
 
 * \param num: 扣除的爱心数数量
 * \return 成功返回true,否则返回false
 */
bool Packages::removeFlloves(DWORD num ,const char *disc)
{
	if(owner->charbase.flloves < num)
	{
		Zebra::logger->debug("%s(%d)爱心数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.flloves,disc);
		return false;
	}
	owner->charbase.flloves-=num;
	zObject::logger(0,0,"鲜花",owner->charbase.flloves,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}

/**
 * \brief 添加爱心数
 * \param num 爱心数数量
 * \param note 提示信息
 */
void Packages::addFlloves(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.flloves+=num;
	zObject::logger(0,0,"爱心",owner->charbase.flloves,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendFlloves(owner, Cmd::INFO_TYPE_GAME , num, "你得爱心数：");
		}       
	}
}

bool Packages::removeAngel(DWORD num ,const char *disc)
{
	if(owner->charbase.angel < num)
	{
		Zebra::logger->debug("%s(%d)翅膀点数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.angel,disc);
		return false;
	}
	owner->charbase.angel-=num;
	zObject::logger(0,0,"翅膀",owner->charbase.angel,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	
	return true;
}

void Packages::addAngel(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.angel+=num;
	zObject::logger(0,0,"翅膀",owner->charbase.angel,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendAngel(owner, Cmd::INFO_TYPE_GAME , num, "你得翅膀数：");
		}       
	}
}

bool Packages::removeHanbing(DWORD num ,const char *disc)
{
	if(owner->charbase.hanbing < num)
	{
		Zebra::logger->debug("%s(%d)寒冰点数不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.hanbing,disc);
		return false;
	}
	owner->charbase.hanbing-=num;
	zObject::logger(0,0,"寒冰",owner->charbase.hanbing,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));

	return true;
}

void Packages::addHanbing(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	owner->charbase.hanbing+=num;
	zObject::logger(0,0,"寒冰",owner->charbase.hanbing,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendHanbing(owner, Cmd::INFO_TYPE_GAME , num, "你得寒冰数：");
		}       
	}
}

/**     
 * \brief 扣除金子
 *      
 
 * \param num: 扣除的金子数量
 * \param need: 是否需要加成(股票扣钱不能有任何加成)
 * \return 成功返回true,否则返回false
 */
bool Packages::removeGold(DWORD num ,const char *disc , bool need)
{
	if(owner->charbase.gold < num)
	{
		Zebra::logger->debug("%s(%d)金币不足,扣除失败,需要%d,现有%d,描述:%s",owner->name,owner->id,num,owner->charbase.gold,disc);
		return false;
	}
	owner->charbase.gold-=num;
	zObject::logger(owner->charbase.accid,owner->charbase.level,"金币",owner->charbase.gold,num,0,owner->id,owner->name,0,NULL,disc,NULL,0,0);
	Cmd::stMainUserDataUserCmd send;
	owner->full_t_MainUserData(send.data);
	owner->sendCmdToMe(&send,sizeof(send));
	//如果是股票扣钱则存储,否则不用
	if(!need)
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
	//暂时去掉加成效果
	/*
	//DWORD overman_id=owner->getOverMan();
	//加成处理部分
	if(need)
	{
		if(overman_id)
		{
			DWORD tick=(DWORD)(num*0.2); 
			SceneUser *pUser = SceneUserManager::getMe().getUserByID(overman_id);
			if(pUser)
			{
				std::string disc = "消费奖励:"; 
				disc += owner->name;
				pUser->packs.addSalary(tick,disc.c_str(),owner->name);
			}
			else
			{
				Cmd::Session::t_OvermanSalaryAdd add;
				add.id=overman_id;
				add.salary=tick;
				strncpy(add.name,owner->name,MAX_NAMESIZE);
				sessionClient->sendCmd(&add, sizeof(add));
			}
		}
		Cmd::Session::t_SpendGold_SceneSession s;
		s.userID = owner->id;
		s.gold = num;
		sessionClient->sendCmd(&s, sizeof(s));
	}
	// */


	return true;
}
/**
 * \brief 添加金币
 * \param num 金币数量
 * \param note 提示信息
 */
void Packages::addGold(DWORD num, const char *disc ,  const char *note, bool notify,bool pack)
{       
	using namespace Cmd;
	//soke 金子上限10W锭
	DWORD Goldx = 1000000000;
	if ((owner->charbase.gold + num) > Goldx)
	{
		owner->charbase.gold+= (Goldx - owner->charbase.gold);
		zObject::logger(owner->charbase.accid,owner->charbase.level,"金子",owner->charbase.gold,(Goldx - owner->charbase.gold),1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				Channel::sendGold(owner, Cmd::INFO_TYPE_GAME , (Goldx - owner->charbase.gold), "你得到金子");
			}       
			else                    
			{       
				Channel::sendGold(owner, Cmd::INFO_TYPE_GAME , (Goldx - owner->charbase.gold), disc);
			}
		}
	}
	else
	{
		owner->charbase.gold+=num;
		zObject::logger(owner->charbase.accid,owner->charbase.level,"金子",owner->charbase.gold,num,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
		Cmd::stMainUserDataUserCmd send;
		owner->full_t_MainUserData(send.data);
		owner->sendCmdToMe(&send,sizeof(send));
		owner->save(Cmd::Record::OPERATION_WRITEBACK);
		if (notify)
		{
			if (note == NULL)
			{
				//Channel::sendSys(owner, Cmd::INFO_TYPE_GAME, "你得到金子%d",num);
				Channel::sendGold(owner, Cmd::INFO_TYPE_GAME , num, "你得到金子");
			}       
			else                    
			{       
				Channel::sendGold(owner, Cmd::INFO_TYPE_GAME , num, disc);
			}
		}
	}
	if(pack && !(owner->charbase.bitmask & CHARBASE_VIP))
	{
		owner->charbase.bitmask |= CHARBASE_VIP;
		//vip用户赠送一个包裹
		owner->packs.store.goldstore(owner);
	}
}
/**
 * \brief 添加银子
 * \param num 银子数量
 * \param note 提示信息
 */
void Packages::addMoney(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	DWORD realGet = 0;
	zObject *gold = getGold();
	//temp solution, just for record before
	if (!gold) {
		gold = zObject::create(objectbm.get(665), 0);
		if (gold) {
			owner->packs.addObject(gold, true, MAIN_PACK);
		}else {
			Zebra::logger->error("创建银子失败");
		}
	}

	if (!gold) return;
	if(gold->base->maxnum - gold->data.dwNum < num)
	{
		realGet = gold->base->maxnum - gold->data.dwNum;
		gold->data.dwNum = gold->base->maxnum;
	}
	else
	{
		gold->data.dwNum += num;
		realGet = num;
	}
	zObject::logger(owner->charbase.accid,gold->data.qwThisID,gold->data.strName,gold->data.dwNum,realGet,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	stRefCountObjectPropertyUserCmd setgold;
	setgold.qwThisID=gold->data.qwThisID;
	setgold.dwNum=gold->data.dwNum;
	owner->sendCmdToMe(&setgold,sizeof(setgold));

	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendMoney(owner, Cmd::INFO_TYPE_GAME , realGet, "你得到银子");
		}       
		else                    
		{       
			Channel::sendMoney(owner, Cmd::INFO_TYPE_GAME, realGet, "%s" , note);
		}
	}
}

/**
 * \brief 挖宝添加银子
 * \param num 挖宝银子数量
 * \param note 提示信息
 */
void Packages::addMoney2(DWORD num, const char *disc ,  const char *note, bool notify)
{       
	using namespace Cmd;
	DWORD realGet = 0;
	zObject *gold = getGold();
	//temp solution, just for record before
	if (!gold) 
	{
		gold = zObject::create(objectbm.get(665), 0);
		if (gold) 
		{
			owner->packs.addObject(gold, true, MAIN_PACK);
		}
		else 
		{
			Zebra::logger->error("创建银子失败");
		}
	}

	if (!gold) return;
	if(gold->base->maxnum - gold->data.dwNum < num)
	{
		realGet = gold->base->maxnum - gold->data.dwNum;
		gold->data.dwNum = gold->base->maxnum;
	}
	else
	{
		gold->data.dwNum += num;
		realGet = num;
	}
	zObject::logger(owner->charbase.accid,gold->data.qwThisID,gold->data.strName,gold->data.dwNum,realGet,1,0,NULL,owner->id,owner->name,disc,NULL,0,0);
	stRefCountObjectPropertyUserCmd setgold;
	setgold.qwThisID=gold->data.qwThisID;
	setgold.dwNum=gold->data.dwNum;
	owner->sendCmdToMe(&setgold,sizeof(setgold));

	if (notify)
	{
		if (note == NULL)
		{
			Channel::sendMoney(owner, Cmd::INFO_TYPE_GAME , realGet, "挖到银子");
		}       
		else                    
		{       
			Channel::sendMoney(owner, Cmd::INFO_TYPE_GAME, realGet, "%s" , note);
		}
	}
}

/**
 * \brief 遍历查找并合并物品，将结果通知客户端
 * \param o 物品对象
 * \return true 继续遍历， false 终止遍历
 */
bool Combination::exec(zObject* o) //拾取和商店购买叠加 恋恋修复 商城购买 背包如果有非绑定的 都变成非绑定
{
	if (o->data.dwObjectID == _get->data.dwObjectID && o->data.upgrade == _get->data.upgrade && o->data.dwNum < o->base->maxnum)
	 {
		o->data.dwNum += _get->data.dwNum;
		if (o->data.dwNum > o->base->maxnum) {
			_num += (o->base->maxnum - (o->data.dwNum - _get->data.dwNum) );
			_get->data.dwNum = o->data.dwNum - o->base->maxnum;
			o->data.dwNum = o->base->maxnum;

			Cmd::stRefCountObjectPropertyUserCmd status;
			status.qwThisID = o->data.qwThisID;
			status.dwNum = o->data.dwNum;
			_user->sendCmdToMe(&status, sizeof(status));	
			return true;
		}


		_num += _get->data.dwNum;
		_get->data.dwNum = 0;

		Cmd::stRefCountObjectPropertyUserCmd status;
		status.qwThisID = o->data.qwThisID;
		status.dwNum = o->data.dwNum;
		_user->sendCmdToMe(&status, sizeof(status));	

		return false;
	}

	return true;
}



/**
 * \brief 遍历查找并合并物品，将结果通知客户端
 * \param o 物品对象
 * \return true 继续遍历， false 终止遍历
 */
bool Combination2::exec(zObject* o) //拾取和商店购买叠加 恋恋修复 商城购买 背包如果有非绑定的 都变成非绑定
{
	if (o->data.dwObjectID == _get->data.dwObjectID && o->data.upgrade == _get->data.upgrade && o->data.dwNum < o->base->maxnum && o->data.bind == _get->data.bind)
	 {
		o->data.dwNum += _get->data.dwNum;
		if (o->data.dwNum > o->base->maxnum) {
			_num += (o->base->maxnum - (o->data.dwNum - _get->data.dwNum) );
			_get->data.dwNum = o->data.dwNum - o->base->maxnum;
			o->data.dwNum = o->base->maxnum;

			Cmd::stRefCountObjectPropertyUserCmd status;
			status.qwThisID = o->data.qwThisID;
			status.dwNum = o->data.dwNum;
			_user->sendCmdToMe(&status, sizeof(status));	
			return true;
		}


		_num += _get->data.dwNum;
		_get->data.dwNum = 0;

		Cmd::stRefCountObjectPropertyUserCmd status;
		status.qwThisID = o->data.qwThisID;
		status.dwNum = o->data.dwNum;
		_user->sendCmdToMe(&status, sizeof(status));	

		return false;
	}

	return true;
}
/*
 * \param kind		说明删除物品的原因
 *			如：改造，升级，etc
 *
 *
bool ClearPack::exec(zObject* ob, char *kind)
{
	char event[128]={0};
	
	if (kind == NULL)
		strcpy(event,"改造物品删除");
	else
	{
		strcpy(event, kind);
		strcat(event, "物品删除");
	}

	zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,0,const_cast<Packages* >(_ps)->getOwner()->id,const_cast<Packages* >(_ps)->getOwner()->name,0,NULL,event,ob->base,ob->data.kind,ob->data.upgrade);
	const_cast<Packages* >(_ps)->removeObject(ob); //notify and delete
	return true;
}*/
bool ClearPack::exec(zObject* ob)
{
	zObject::logger(ob->createid,ob->data.qwThisID,ob->data.strName,ob->data.dwNum,ob->data.dwNum,0,const_cast<Packages* >(_ps)->getOwner()->id,const_cast<Packages* >(_ps)->getOwner()->name,0,NULL,"改造物品删除",ob->base,ob->data.kind,ob->data.upgrade);
	const_cast<Packages* >(_ps)->removeObject(ob); //notify and delete
	return true;
}


DropFromPack::DropFromPack(const Packages* ps, const Param& p) : _ps(ps), _p(p)
{ 
	int num = p.pack->size() -p.pack->space();
	_begin = (num < p.drop_num)?0:zMisc::randBetween(0, num - p.drop_num);
	_pos = 0;
}
	

bool DropFromPack::exec(zObject* ob)
{
/*
//TODOBYLQY
	std::vector<zObject *> temp_vec;
	int begin = 0;
	std::set<zObject *>::iterator iter;
	if(mainpack > 0)
	{
		for(iter = packs.main.getAllset().begin(); iter != packs.main.getAllset().end() ; iter ++)
		{
			//TODO 其它不可掉落物品
			if((*iter)->data.upgrade > 5 || (*iter)->data.bind || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_PACKAGE || (*iter)->data.pos.y == Cmd::EQUIPCELLTYPE_MAKE || (*iter)->base->kind==ItemType_MASK)
			{
				continue;
			}
			temp_vec.push_back(*iter);
		}
		if(mainpack < (int)temp_vec.size())
		{
			begin = zMisc::randBetween(0 , temp_vec.size() - mainpack);
		}
		else
		{
			mainpack = temp_vec.size();
		}
		for(int i = begin; i < mainpack ;  i ++)
		{
			this->packs.moveObjectToScene(&*temp_vec[i] , this->getPos());
		}
	}
*/	
	if( /*ob->data.upgrade > 5 || */ob->data.bind || 
		ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_PACKAGE || 
		ob->data.pos.ypos() == Cmd::EQUIPCELLTYPE_MAKE || ob->base->kind==ItemType_MASK ||
		ob->base->kind==ItemType_Quest) 
	{
		//didn't drop
		return true;
	}
	++_pos;
	if (_pos >= _begin  && _p.drop_num > 0) {
		const_cast<Packages*>(_ps)->moveObjectToScene(ob, _p.pos);
		--_p.drop_num;
	}
	
	return true;
}

//////////////宠物包裹//////////////
PetPack::PetPack()
:Package(Cmd::OBJECTCELLTYPE_PET, 0, 8, 10),available(0)
{
	setSpace(0);
}
PetPack::~PetPack() {}

bool PetPack::checkAdd(SceneUser* pUser, zObject* object, WORD x, WORD y)
{
	if (x>W || y>H) return false;

	int pos = W*y + x;
	if (pos>available) return false;

	return Package::checkAdd(pUser, object, x, y);
}

void PetPack::setAvailable(WORD s)
{
	WORD old = available;
	available = s;

	WORD n = space();
	if (s>old)
		n += s-old;
	else if (old>s)
		n -= old-s;

	setSpace(n);
}

WORD PetPack::size() const
{
	return available;
}

bool PetPack::isEmpty() const
{
	return space()==size();
}

//////////////////////////////////////////////////////////
RecastPack::RecastPack()
:Package(Cmd::OBJECTCELLTYPE_RECAST, 0, RECAST_PACK_WIDTH, RECAST_PACK_HEIGHT)
{
}

RecastPack::~RecastPack()
{
}

AnnexPack::AnnexPack()
:Package(Cmd::OBJECTCELLTYPE_ANNEX, 0, RECAST_PACK_WIDTH, RECAST_PACK_HEIGHT)
{

}

AnnexPack::~AnnexPack()
{

}

HandbookPack::HandbookPack()
:Package(Cmd::OBJECTCELLTYPE_HANDBOOK, 0, RECAST_PACK_WIDTH, RECAST_PACK_HEIGHT)
{

}

HandbookPack::~HandbookPack()
{

}
FabaoPack::FabaoPack()
:Package(Cmd::OBJECTCELLTYPE_FABAO, 0, RECAST_PACK_WIDTH, RECAST_PACK_HEIGHT)
{

}

FabaoPack::~FabaoPack()
{

}

FujianPack::FujianPack()
:Package(Cmd::OBJECTCELLTYPE_FUJIAN, 0, FUJIAN_PACK_WIDTH, FUJIAN_PACK_HEIGHT)
{
}

FujianPack::~FujianPack()
{
}

YuanshenPack::YuanshenPack()
:Package(Cmd::OBJECTCELLTYPE_YUANSHEN, 0, RECAST_PACK_WIDTH, RECAST_PACK_HEIGHT)
{

}
YuanshenPack::~YuanshenPack()
{

}





