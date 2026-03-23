/**
*\file		GuiNpcMake.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    打造、合成、升级、魂魄合成、镶嵌、打孔、食物烹饪对话框
*
* 
*
*/
#include "public.h"
//#include <boost/regex.hpp>
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./RoleItem.h"
#include "./RoleSkill.h"
#include "./MainCharacter.h"
#include "./GuiNpcDialog.h"
#include "./Country.h"
#include "../Media/SoundManager.h"
#include ".\guinpcmake.h"
#include "SafetyUser.h"
//#include "../greta/regexpr2.h"
//using namespace regex;

struct stGroupInfo {
	byte    id;
	char    name[MAX_NAMESIZE];
};

struct stItemTypeInGroup {
	byte   groupid;
	byte   itemtype;
};

static   DWORD  g_nPos = 0;

///材料等级称号
char g_szNumber[][8]={
	{""},
	{"一等"},
	{"二等"},
	{"三等"},
	{"四等"},
	{"五等"},
	{"六等"},
	{"七等"},
	{"八等"},
	{"九等"},
	{"十等"},
	{"十一等"},
};

///打造物品类型名字
stGroupInfo g_GroupInfo[] = 
{
	{Body,"服装"},
	{Helm,"头盔"},
	{Attack,"武器"},
	{Shield,"盾牌"},
	{Caestus,"腰带"},
	{Cuff,"护腕"},
	{Shoes,"鞋子"},
	{Necklace,"项链"},
	{Fing,"戒指"},
	{Resource,"原料"},
	{Stone,"宝石"},
	{Food1,"清蒸"},
	{Food2,"红烧"},
	{Food3,"煮"},
	{Food4,"烤"},
	{Leechdom,"药物"},
	{Package,"包裹"},
	{Paper,"卷轴"},
	{FashionBody,"时装"},
	{HorseEquip,"宝马铠甲"},
	{HighFashionBody,"高级时装"},
	{HorseShoe,"马蹄铁"},
	{HorseRope,"马缰绳"},
	{HorseSaddle,"马鞍"},
	{HorseSafe,"护马甲"},
	{HorseIron,"马镫"},
	{HorseFashion,"马牌"},
	{StaffFlag,"战旗(法师)"},
	{CrossFlag,"战旗(弓手)"},
	{Flag,"战旗(仙术)"},
	{StickFlag,"战旗(召唤)"},
	{BladeFlag,"战旗(战士)"},
	{DaggerFlag,"战旗(刺客)"},
	{SwordFlag,"战旗(卫士)"},
	{QiangFlag,"长枪"},
	{DuanFlag,"短枪"},
	{ShenBiFlag,"神兵"},
	{ShenShFlag,"神兽"},
	{AssistantFlag,"羽翼"}
};
///打造物品类型与物品基本类型对应表
stItemTypeInGroup g_ItemtypeInGroup [] =
{
	{Body,ItemType_ClothBody},
	{Body,ItemType_FellBody},
	{Body,ItemType_MetalBody},
	{Helm,ItemType_Helm},
	{Attack,ItemType_Blade},
	{Attack,ItemType_Sword},
	{Attack,ItemType_Bow},
	{Attack,ItemType_Axe},
	{Attack,ItemType_Hammer},
	{Attack,ItemType_Fan},
	{Attack,ItemType_Staff},
	{Attack,ItemType_Stick},
	{Attack,ItemType_Dagger},
	{Attack,ItemType_NewSword},
	{Shield,ItemType_Shield},
	{Caestus,ItemType_Caestus},
	{Cuff,ItemType_Cuff},
	{Shoes,ItemType_Shoes},
	{Necklace,ItemType_Necklace},
	{Fing,ItemType_Fing},
	{Resource,ItemType_Resource},
	{Stone,ItemType_LevelUp},
	{Stone,ItemType_Quest},
	{Food1,51},
	{Food2,52},
	{Food3,53},
	{Food4,54},
	{Leechdom,ItemType_Leechdom},
	{Package,ItemType_Pack},
	{Paper,ItemType_Scroll},
	{FashionBody,ItemType_FashionBody},
	{HorseEquip,ItemType_BMW},
	{HighFashionBody,ItemType_HighFashionBody},
	{HorseShoe,ItemType_HorseShoe},
	{HorseRope,ItemType_HorseRope},
	{HorseSaddle,ItemType_HorseSaddle},
	{HorseSafe,ItemType_HorseSafe},
	{HorseIron,ItemType_HorseIron},
	{HorseFashion,ItemType_HorseFashion},
	{StaffFlag,ItemType_StaffFlag},
	{Shield,ItemType_breastplate},
	{CrossFlag,ItemType_CrossFlag},
	{Flag,ItemType_Flag},
	{StickFlag,ItemType_StickFlag},
	{BladeFlag,ItemType_BladeFlag},
	{DaggerFlag,ItemType_DaggerFlag},
	{SwordFlag,ItemType_SwordFlag},
	{QiangFlag,ItemType_LongGun},
	{DuanFlag,ItemType_Handgun},
	{ShenBiFlag,ItemType_Shenbing0},
	{ShenBiFlag,ItemType_Shenbing1},
	{ShenBiFlag,ItemType_Shenbing2},
	{ShenShFlag,ItemType_Shenshou1},
	{ShenShFlag,ItemType_Shenshou2},
	{AssistantFlag,ItemType_Chibang},
};

///打造物品的配置文件
const char* c_szMakeObjectFileName = "datas\\MakeObject.ini";

///打造物品的配置文件的标识表
const stMakeDataMainItem  g_MainItemList[] = {
	{CGuiNpcDialog::eChange,eMaterial,HCCL},

	{CGuiNpcDialog::eMake,eMaterial,DZCL},
	{CGuiNpcDialog::eMake,eRate,DZCL},
	{CGuiNpcDialog::eMake,eRateItem,DZGL},

	{CGuiNpcDialog::eUp,eMaterial,SJCL},
	{CGuiNpcDialog::eUp,eRate,SJJL},
	{CGuiNpcDialog::eUp,eRateItem,SJGL},

	{CGuiNpcDialog::eKaChange,eMaterial,LHHCCL},

	{CGuiNpcDialog::eKaMake,eMaterial,LHXQCL},
	{CGuiNpcDialog::eKaMake,ePrice,LHXQJG},

	{CGuiNpcDialog::eHoleMake,eMaterial,DKCL},
	{CGuiNpcDialog::eHoleMake,ePrice,DKJG},
	
	{CGuiNpcDialog::eBackout,eMaterial,DZCL},
	{CGuiNpcDialog::eBackout,eRateItem,DZGL},

	{CGuiNpcDialog::eMakeFood,eMaterial,DZCL},
	{CGuiNpcDialog::eMakeDrug,eMaterial,DZCL},

	//soke  改造对应
	{CGuiNpcDialog::eModifyAttri,eMaterial,MDCL},
	{CGuiNpcDialog::eModifyAttri,ePrice,MDJG},

	{CGuiNpcDialog::eAddReBind,eMaterial,MDCL},
	{CGuiNpcDialog::eAddReBind,ePrice,MDJG},

	{CGuiNpcDialog::eAddReMaker,eMaterial,MDCL},
	{CGuiNpcDialog::eAddReMaker,ePrice,MDJG},

	{CGuiNpcDialog::eAddAddMaker,eMaterial,MDCL},
	{CGuiNpcDialog::eAddAddMaker,ePrice,MDJG},

	{CGuiNpcDialog::eAddModifySoulA,eMaterial,MDCL},
	{CGuiNpcDialog::eAddModifySoulA,ePrice,MDJG},

	{CGuiNpcDialog::eAddLVSoullineMD,eMaterial,MDCL},
	{CGuiNpcDialog::eAddLVSoullineMD,ePrice,MDJG},

	{CGuiNpcDialog::eAddModifyDxSoulA,eMaterial,MDCL},
	{CGuiNpcDialog::eAddModifyDxSoulA,ePrice,MDJG},

	{CGuiNpcDialog::eAddMDSkillA,eMaterial,MDCL},
	{CGuiNpcDialog::eAddMDSkillA,ePrice,MDJG},

	{CGuiNpcDialog::eAddEquipRound,eMaterial,MDCL},
	{CGuiNpcDialog::eAddEquipRound,ePrice,MDJG},

    //soke  马匹装备重新锻造
	{CGuiNpcDialog::eAddMDMake,eMaterial,MDCL},
	{CGuiNpcDialog::eAddMDMake,ePrice,MDJG},

	//soke  祝福改造对应
	{CGuiNpcDialog::eAddEquipMake,eMaterial,MDC2L},
	{CGuiNpcDialog::eAddEquipMake,ePrice,MDJ2G},

	//soke  马匹祝福改造对应
	{CGuiNpcDialog::eAddEquipZsMake,eMaterial,MDC3XL},
	{CGuiNpcDialog::eAddEquipZsMake,ePrice,MDJ3XG},

	//soke  升级改造对应
	/* {CGuiNpcDialog::eAddEquipMaklv,eMaterial,MDC2XL},
	{CGuiNpcDialog::eAddEquipMaklv,ePrice,MDJ2XG}, */
	{CGuiNpcDialog::eAddEquipMaklv,eMaterial,DZCL},
	{CGuiNpcDialog::eAddEquipMaklv,eRate,DZCL},
	{CGuiNpcDialog::eAddEquipMaklv,eRateItem,DZGL},

};



void ResponseMake(stResponsePropertyUserCmd* pCmd)
{
	switch(pCmd->kind) 
	{
	case COMPOSE:
	case UPGRADE:
	case ENCHANCE:
		{
			if (GetGameGuiManager()->m_guiMakeTo1)
			{
				if (g_nPos==GetGameGuiManager()->m_guiMakeTo1->m_nPos)
				{
					GetGameGuiManager()->m_guiMakeTo1->Response(pCmd);
					return;
				}
			}
			if (GetGameGuiManager()->m_guiMakeTo2)
			{
				if (g_nPos==GetGameGuiManager()->m_guiMakeTo2->m_nPos)
				{
					GetGameGuiManager()->m_guiMakeTo2->Response(pCmd);
					return;
				}
			}
			
		}
		break;
	case MAKE:
	case HOLE:
	case DECOMPOSE:
	    {
			if(GetGameGuiManager()->m_guiNpcMake)
			{
				GetGameGuiManager()->m_guiNpcMake->Response(pCmd);
			}
		}
		break;
	case MODIFY: //soke 改造参数
	case MAKEZS: //soke 装备祝福参数
	case MAKEZQ: //soke 马匹装备祝福参数
	case MAKELV: //soke 装备升级参数
		{
			if(GetGameGuiManager()->m_guiNpcMake)
			{
				GetGameGuiManager()->m_guiNpcMake->Response1(pCmd);
			}
		}
		break;
	}
}

/**
 * \brief 解析从服务器端发送过来的打造物品
 * 
 * 解析从服务器端发送过来的打造物品
 * 
 * \param aObjects : 保存打造物品的数据
 * \param pszString : 从服务器端发送过来的打造物品的字符串
 * \param state : 打造类型
 * \return 无
 */
template < class tVecObject >
void ParseMakeString(tVecObject & aObjects,const char* pszString,BYTE state)
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	std::vector<std::stack_string, std::stack_allocator<std::stack_string> > aParams1;

	_parse_str(aParams1,pszString,';');
	std::string strTemp;

	for(size_t i=0;i<aParams1.size();++i)
	{
		const char* pszTemp = aParams1[i].c_str();

		SetFrameAllocator fa;
		std::vector<std::stack_string, std::stack_allocator<std::stack_string> > results;
		std::vector< std::stack_string, std::stack_allocator<std::stack_string> > results1;
		_parse_str(results,pszTemp,'[');

		if(results.size() == 2)
		{
			if(results[1].size())
				results[1].resize(results[1].size()-1);

			pszTemp = results[1].c_str();
			std::vector<int,std::stack_allocator<int> > aParams2;
			if(stricmp("id",results[0].c_str() ) == 0)
			{
				// 按 ID 查找物品
				_parse_str(results1,results[1].c_str(),',');

				for( size_t k = 0; k < results1.size(); ++k )
				{
					_parse_str_num(aParams2,results1[k].c_str());
					for (size_t h=aParams2[0];h<=aParams2[aParams2.size()-1];++h)
					{
						ObjectBase_t* pObject = GetObjectBase(h);
						if(pObject)
						{
							//if (pObject->price!=0)
							aObjects.push_back(pObject);
						}
					}
				}

			}
			else if(stricmp("type",results[0].c_str() ) == 0 )
			{
				// 按 类型 查找物品
				_parse_str(results1,results[1].c_str(),',');

				for( size_t j = 0; j < results1.size(); ++j )
				{
					_parse_str_num(aParams2,results1[j].c_str());

					for( size_t k = aParams2[0]; k <= aParams2[aParams2.size()-1]; ++k )
					{
						for(size_t h = 0 ; h < g_tableObjectBase.size();++h)
						{
							if(g_tableObjectBase[h].dwType == k)
							{
								/*switch(state) {
								case CGuiNpcDialog::eUp:
									if (g_tableObjectBase[h].dwUp!=0)
										aObjects.push_back(&g_tableObjectBase[h]);
									break;
								case CGuiNpcDialog::eMake:
									if (g_tableObjectBase[h].dwMake!=0)
										aObjects.push_back(&g_tableObjectBase[h]);
									break;
								default:
									if (g_tableObjectBase[h].price!=0)
										aObjects.push_back(&g_tableObjectBase[h]);
								break;
								}*/
								aObjects.push_back(&g_tableObjectBase[h]);
							}
						}
					}
				}
			}
		}
	}

	FUNCTION_END;
}

/**
 * \brief 两个值中取大值计算
 * 
 * 两个值中取大值计算(打造的几率计算)
 * 
 * \param first : 第一个值
 * \param second : 第二个值
 * \param result : 返回值
 * \param level : 材料等级
 * \return 无
 */
template<typename T>
static void additive(T first, T second, T& result, int level)
{
	FUNCTION_BEGIN;

	if (first && second) {
		//result = std::max(first, second) + 0.1*std::min(first, second)	
		T max = first, min = second, grade = level & 0xff;
		if (max < min) {
			max = second;
			min = first;
			grade = level >> 8;
		}

		result =  max + static_cast<T>(min*0.1*grade);	
	}else {
		result = first + second;
	}


	FUNCTION_END;
}


/**
 * \brief 打造的几率计算
 * 
 * 打造的几率计算
 * 
 * \param first : 材料一
 * \param second : 材料二
 * \param dest : 打造物品
 * \return 是否成功
 */
bool do_compose(t_Object* first, t_Object* second, t_Object* dest)
{
	FUNCTION_BEGIN;
	/* zzf 2005.7.12
	int level = ( (first->upgrade & 0xff) << 8 ) | (second->upgrade & 0xff);

	dest->needLevel = max(first->needLevel, second->needLevel);
	additive(first->hpleech, second->hpleech, dest->hpleech, level);
	additive(first->mpleech, second->mpleech, dest->mpleech, level);
	additive(first->hpleech_value, second->hpleech_value, dest->hpleech_value, level);
	additive(first->mpleech_value, second->mpleech_value, dest->mpleech_value, level);

	additive(first->hptomp, second->hptomp, dest->hptomp, level);
	additive(first->dhpp, second->dhpp, dest->dhpp, level);
	additive(first->dmpp, second->dmpp, dest->dmpp, level);

	additive(first->ehpp, second->ehpp, dest->ehpp, level);
	additive(first->reflectp, second->reflectp, dest->reflectp, level);

	additive(first->invtarget, second->invtarget, dest->invtarget, level);
	additive(first->incgold, second->incgold, dest->incgold, level);

	additive(first->cold, second->cold, dest->cold, level);
	additive(first->poison, second->poison, dest->poison, level);
	additive(first->petrify, second->petrify, dest->petrify, level);
	additive(first->chaos, second->chaos, dest->chaos, level);
	additive(first->blind, second->blind, dest->blind, level);

	additive(first->colddef, second->colddef, dest->colddef, level);
	additive(first->poisondef, second->poisondef, dest->poisondef, level);
	additive(first->petrifydef, second->petrifydef, dest->petrifydef, level);
	additive(first->chaosdef, second->chaosdef, dest->chaosdef, level);
	additive(first->blinddef, second->blinddef, dest->blinddef, level);

	additive(first->doublexp, second->doublexp, dest->doublexp, level);
	additive(first->mf, second->mf, dest->mf, level);
	additive(first->charm, second->charm, dest->charm, level);
	additive(first->sleight, second->sleight, dest->sleight, level);
	*/
	return true;


	FUNCTION_END;
}

/**
 * \brief 镶嵌魂魄后物品后的属性加成
 * 
 * 镶嵌魂魄后物品后的属性加成
 * 
 * \param dest : 镶嵌魂魄的物品
 * \param src : 魂魄
 * \return 是否成功
 */
bool do_enchance(t_Object* dest, t_Object* src)
{
	FUNCTION_BEGIN;

	/* zzf 2005.7.12
	dest->hpleech += src->hpleech;
	dest->mpleech += src->mpleech;
	dest->hpleech_value += src->hpleech_value;
	dest->mpleech_value += src->mpleech_value;

	if (dest->needLevel < src->needLevel) {
		dest->needLevel = src->needLevel;
	}
	dest->hptomp += src->hptomp;
	dest->dhpp += src->dhpp;
	dest->dmpp += src->dmpp;

	dest->ehpp += src->ehpp;
	dest->reflectp += src->reflectp;

	dest->invtarget += src->invtarget;
	dest->incgold += src->incgold;

	dest->cold += src->cold;
	dest->poison += src->poison;
	dest->petrify += src->petrify;
	dest->chaos += src->chaos;
	dest->blind += src->blind;

	dest->colddef += src->colddef;
	dest->poisondef += src->poisondef;
	dest->petrifydef += src->petrifydef;
	dest->chaosdef += src->chaosdef;
	dest->blinddef += src->blinddef;

	dest->doublexp += src->doublexp;
	dest->mf += src->mf;
	dest->charm += src->charm;
	dest->sleight += src->sleight;

	dest->socket[0] = 1;
	*/
	return true;

	FUNCTION_END;
}

/**
 * \brief 转盘的构造函数
 * 
 * 转盘的构造函数
 * 
 * \param pt : 转盘显示的位置
 * \return 无
 */
CMakeGuess::CMakeGuess(stPointI pt)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_Ini.Open("data\\datas.pak",c_szMakeObjectFileName);
	m_ptRender = pt;
	m_bSmDialog = true;
	InitData();

	FUNCTION_END;
}

/**
 * \brief 刷新转盘的背景
 * 
 * 刷新转盘的背景
 * 
 * \param fElapsedTime : 刷新的时间
 * \return 无
 */
void CMakeGuess::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	IBitmap* pBmp = GetDevice()->FindBitmap(&stResourceLocation("data\\Interfaces.gl",15,1));
	if(pBmp)
	{
		pBmp->Render(m_ptRender.x,m_ptRender.y);
	}

	FUNCTION_END;
}

/**
 * \brief 初始化转盘数据
 * 
 * 初始化转盘数据
 * 
 * \return 无
 */
void CMakeGuess::InitData()
{
	FUNCTION_BEGIN;

	m_bStart=false;
	m_MakeResult=0;
	m_fRunTime=0.0f;
	m_fAngle = 0.0f;
	m_fEnd=0.0f;

	m_ptAngle.x=0;
	m_ptAngle.y=0;

	FUNCTION_END;
}

/**
 * \brief 设置转盘转的控制数据
 * 
 * 设置转盘转的控制数据
 * 
 * \return 无
 */
void CMakeGuess::SetPlayCtrl()
{
	FUNCTION_BEGIN;

	char szText1[20]={0};
	char szText2[20]={0};
	m_playCtrl.clear();

	
	std::vector<UINT, std::stack_allocator<UINT> > info;
	_parse_str_num(info,m_Ini.GetString(_itoa(DZSPEED,szText1,10),_itoa(m_eState,szText2,10),"10,10,10").c_str());

	if (info.size()>0)
	{
		m_fRunTime=(float)info[0];
		for (int i=1;i<info.size();i+=2)
		{
			stSpeed st;
			st.fTime=(float)info[i];
			st.angle=info[i+1];
			m_playCtrl.push_back(st);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 设置转盘结束的角度范围
 * 
 * 设置转盘结束的角度范围
 * 
 * \param value : 打造的结果
 * \return 无
 */
void CMakeGuess::SetEndAngle(DWORD value)
{
	FUNCTION_BEGIN;

	char szText1[20]={0};
	char szText2[20]={0};

	std::vector<UINT, std::stack_allocator<UINT> > info;
	_parse_str_num(info,m_Ini.GetString(_itoa(DZANGLE,szText1,10),_itoa(value,szText2,10),"1,10").c_str());

	if (info.size()>0)
	{
		m_ptAngle.x=info[0];
		m_ptAngle.y=info[1];
	}
	

	FUNCTION_END;
}

/**
 * \brief 转盘的数据刷新
 * 
 * 转盘的数据刷新
 * 
 * \param fElapsedTime : 刷新的时间
 * \return 是否处理成功
 */
HRESULT CMakeGuess::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);


	if (IsVisible())
	{
		static float fCloseRunTime = 0.0f;

		m_fRunTime-=fElapsedTime;
		stPointI ptAxis(m_ptRender.x+185,m_ptRender.y+185);

		if (m_bStart)
		{
			int angle=0;
			for (int i=0;i<m_playCtrl.size();i++)
			{
				if (m_playCtrl[i].fTime>m_fRunTime)
					angle=m_playCtrl[i].angle;
				else
					break;
			}
			
			m_fAngle += fElapsedTime * angle2radian(angle);
			if(m_fAngle > 2 * PI)
				m_fAngle -= 2 * PI;

			if ((m_fAngle>angle2radian(m_ptAngle.x))&&(m_fAngle<angle2radian(m_ptAngle.y)))
			{
				if (m_fRunTime-m_fEnd<5.0f)
				{
					m_bStart=false;
					if (GetGameGuiManager()->m_guiNpcMake)
					{
						GetGameGuiManager()->m_guiNpcMake->SendGuessFinish();
					}
				}
			}

			fCloseRunTime=0.0f;
		}
		else
		{	
			fCloseRunTime+=fElapsedTime;
			if (fCloseRunTime>MAKESTOPTIME)
				SetVisible(false);
			
		}
		GetDevice()->SetRotation(ptAxis,m_fAngle);
		IBitmap* pBmp = GetDevice()->FindBitmap(&stResourceLocation("data\\Interfaces.gl",15,0));
		if(pBmp)
		{
			pBmp->Render(m_ptRender.x,m_ptRender.y);
		}
		GetDevice()->IdentityTransform();

	}


	return hr;

	FUNCTION_END;
}

/**
 * \brief 打造数据类构造函数
 * 
 * 打造数据类构造函数
 * 
 * \return 无
 */
CMakeData::CMakeData()
{
	FUNCTION_BEGIN;

	InitData();
	LoadData();

	FUNCTION_END;
}

/**
 * \brief 打造数据类析造函数
 * 
 * 打造数据类析造函数
 * 
 * \return 无
 */
CMakeData::~CMakeData()
{
	FUNCTION_BEGIN;

	InitData();

	FUNCTION_END;
}

/**
 * \brief 初始化打造数据
 * 
 * 初始化打造数据
 * 
 * \return 无
 */
void CMakeData::InitData()
{
	FUNCTION_BEGIN;

	m_Ini.Open("data\\datas.pak",c_szMakeObjectFileName);

	m_Info.reserve(16);

	m_MaterialInfo.clear();
	m_SelectInfo.clear();

	FUNCTION_END;
}

/**
 * \brief 加载打造数据
 * 
 * 加载打造数据
 * 
 * \return 无
 */
void CMakeData::LoadData()
{
	FUNCTION_BEGIN;

	
	

	FUNCTION_END;
}

/**
 * \brief 插入一条当前选择的材料数据
 * 
 * 插入一条当前选择的材料数据
 * 
 * \param p : 材料结构
 * \return 无
 */
void CMakeData::insertSelectData(stMaterialInfo* p)
{
	FUNCTION_BEGIN;

	bool bFind=false;

	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]==p)
		{
			bFind=true;
			break;
		}
	}

	if (!bFind)
		m_SelectInfo.push_back(p);

	FUNCTION_END;
}

/**
 * \brief 删除已选择的一条材料数据
 * 
 * 删除已选择的一条材料数据
 * 
 * \param p : 材料结构
 * \return 无
 */
void CMakeData::deleteSelectData(stMaterialInfo* p)
{
	FUNCTION_BEGIN;

	for (SELECTINFO::iterator pointer=m_SelectInfo.begin();
		pointer!=m_SelectInfo.end();pointer++)
	{
		if ((*pointer)==p)
		{
			m_SelectInfo.erase(pointer);
			break;
		}
	}

	FUNCTION_END;
}

/**
 * \brief 根据材料物品ID查找已经选择的材料数据结构
 * 
 * 查找已经选择的材料数据结构
 * 
 * \param dwID : 材料物品ID
 * \return 材料物品结构
 */
stMaterialInfo* CMakeData::findSelectDataByObjectID(DWORD dwID)
{
	FUNCTION_BEGIN;

	stMaterialInfo* pData=NULL;

	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]->GetObjectBaseID()==dwID)
		{
			pData=m_SelectInfo[i];
			break;
		}
	}
	return pData;


	FUNCTION_END;
}

stMaterialInfo* CMakeData::findKaStoneSelectData()
{
	FUNCTION_BEGIN;

	stMaterialInfo* pData=NULL;

	pData = findSelectDataByObjectID(KASTONE);
	if (pData!=NULL)
		return pData;

	pData = findSelectDataByObjectID(KASTONE1);
	if (pData!=NULL)
		return pData;

	pData = findSelectDataByObjectID(KASTONE2);
	if (pData!=NULL)
		return pData;

	pData = findSelectDataByObjectID(KASTONE3);
	if (pData!=NULL)
		return pData;

	pData = findSelectDataByObjectID(KASTONE4);
	
	return pData;

	FUNCTION_END;
}

/**
 * \brief 根据材料物品结构检查是否是已经选择的材料
 * 
 * 根据材料物品结构检查是否是已经选择的材料
 * 
 * \param pData : 材料物品结构
 * \return 是否已经选择
 */
bool CMakeData::findSelectDataByData(stMaterialInfo* pData)
{
	FUNCTION_BEGIN;

	bool b=false;

	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]==pData)
		{
			b=true;
			break;
		}
	}
	return b;

	FUNCTION_END;
}

/**
 * \brief 根据材料物品ID和材料等级查找已经选择的材料的数量
 * 
 * 根据材料物品ID和材料等级查找已经选择的材料的数量
 * 
 * \param dwID : 材料物品ID
 * \param nLevel : 材料等级
 * \return 材料选择的数量
 */
int CMakeData::findSelectDataByObjectNum(DWORD dwID,int nLevel)
{
	FUNCTION_BEGIN;

	int nRet=0;
	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]->GetObjectBaseID()==dwID)
		{
			if ((m_SelectInfo[i]->nLevel!=nLevel)&&(nLevel!=-1))
				continue;

			nRet+=m_SelectInfo[i]->nUseNum;
		}
	}
	return nRet;

	FUNCTION_END;
}


int CMakeData::findKaStoneNum()
{
	FUNCTION_BEGIN;

	int nRet=0;
	
	nRet += findSelectDataByObjectNum(KASTONE);
	nRet += findSelectDataByObjectNum(KASTONE1);
	nRet += findSelectDataByObjectNum(KASTONE2);
	nRet += findSelectDataByObjectNum(KASTONE3);
	nRet += findSelectDataByObjectNum(KASTONE4);

	return nRet;

	FUNCTION_END;
}


/**
 * \brief 根据材料物品ID查找已经选择的材料最大等级
 * 
 * 根据材料物品ID查找已经选择的材料最大等级
 * 
 * \param dwID : 材料物品ID
 * \return 材料最大等级
 */
int CMakeData::getSelectDataByObjectMaxLevel(DWORD dwID)
{
	FUNCTION_BEGIN;

	int nRet=0;
	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]->GetObjectBaseID()==dwID)
		{
			if (m_SelectInfo[i]->nLevel>nRet)
				nRet=m_SelectInfo[i]->nLevel;
		}
	}
	return nRet;

	FUNCTION_END;
}

/**
 * \brief 根据材料配置文件的标识表得到配置的唯一ID
 * 
 * 根据材料配置文件的标识表得到配置的唯一ID
 * 
 * \param state : 打造类型
 * \param type : 配置类型
 * \return 配置的唯一ID
 */
int  CMakeData::GetMainItem(byte state,enumDataType type)
{
	FUNCTION_BEGIN;

	static int size = sizeof(g_MainItemList)/sizeof(stMakeDataMainItem);

	for (int i=0;i<size;i++)
	{
		if ((g_MainItemList[i].state==state)&&(g_MainItemList[i].type==type))
		{
			return g_MainItemList[i].MainItem;
		}
	}
	return -1;

	FUNCTION_END;
}

/**
 * \brief 根据打造类型得到打造几率增加的物品
 * 
 * 根据打造类型得到打造几率增加的物品
 * 
 * \param type : 打造类型
 * \return 打造几率增加的物品
 */
DWORD  CMakeData::getMakeRateObject(int type)
{
	FUNCTION_BEGIN;

	if (ISARM(type))
		return ARMRATEOBJECT;
	else if(ISBODY(type))
		return BODYRATEOBJECT;
	else if(ISPULLON(type))
		return PULLONRATEOBJECT;
	else if(ISWEAR(type))
		return WEARRATEOBJECT;
	
	return 0;

/**
 * \brief 解析字符串内容,把内容放到数组中
 * 
 * 解析字符串内容,把内容放到数组中
 * 
 * \param nMainItem : 配置文件中的主ID值
 * \param nSubItem : 配置文件中的子ID值
 * \param szDefault : 字符串内容
 * \return 解析的内容数组
 */
	FUNCTION_END;
}
std::vector<UINT, std::stack_allocator<UINT> >& CMakeData::GetInfo(int nMainItem,DWORD nSubItem,char* szDefault)
{
	FUNCTION_BEGIN;

	char szMainItem[4]={0};
	char szSubItem[32]={0};
	if (nMainItem!=DZCL)
		_parse_str_num(m_Info,m_Ini.GetString(_itoa(nMainItem,szMainItem,10),_itoa(nSubItem,szSubItem,10),szDefault).c_str());
	else
	{
		STRINGMAP::iterator  pointer=m_MaterialInfo.find(nSubItem);
		if (pointer!=m_MaterialInfo.end())
			_parse_str_num(m_Info,pointer->second.c_str());
		else
			_parse_str_num(m_Info,szDefault);
	}
	return m_Info;

	FUNCTION_END;
}

/**
 * \brief 插入自动生成的材料字符串内容
 * 
 * 插入自动生成的材料字符串内容
 * 
 * \param dwID : 材料ID
 * \param nStep : 每次累加数
 * \param nMax : 最大使用数
 * \param bAdd : 不同等级的材料是否累加
 * \return 无
 */
void CMakeData::insertMaterial(DWORD dwID,WORD nStep,WORD nMax,byte bAdd)
{
	FUNCTION_BEGIN;
	
	char szText[MAX_NAMESIZE]={0};
	
	sprintf(szText,"%d %d %d",nStep,nMax,bAdd);

	std::string str=szText;

	m_MaterialInfo.insert(STRINGMAP::value_type(dwID,str));

	FUNCTION_END;
}


/**
 * \brief 得到打造白色装备的几率
 * 
 * 得到打造白色装备的几率
 * 
 * \param ob : 物品基本资料结构
 * \return 得到打造白色装备的几率
 */
int CMakeData::odds_of_white( const ObjectBase_t* ob)
{
	FUNCTION_BEGIN;

	//return static_cast<int>((10*odds.per/(odds.per+4) + 10*odds.luck/(odds.luck+10) + 20*odds.material_level + 2*odds.skill_level));
	//return static_cast<int>((10*odds.per/(odds.per+4) + 10*odds.luck/(odds.luck+10) + 25*odds.material_level + 2.5*odds.skill_level));
	//return static_cast<int>((10*odds.per/(odds.per+4) + 10*odds.luck/(odds.luck+10) + 50*odds.material_level + 3*odds.skill_level));
	return static_cast<int>((20*odds.per/(odds.per+4) + 20*odds.luck/(odds.luck+10) + 100*odds.material_level + 10*odds.skill_level));

	FUNCTION_END;
}

/**
* \brief 得到打造兰色装备的几率
* 
* 得到兰色白色装备的几率
* 
* \param ob : 物品基本资料结构
* \return 得到兰色白色装备的几率
*/
int CMakeData::odds_of_blue( const ObjectBase_t* ob)
{
	FUNCTION_BEGIN;

	/*zzf 2005.7.12

	if (odds.material_level > 5.0f) {
		return 1000;
	}

	int odds_num = static_cast<int>( 10*pow(odds.material_level, 5) + 0.1*ob->bluerating - 9);
	return odds_num;
	*/

	int odds_num = ob->bluerating;
	//if (_make) odds = static_cast<int>( pow(_odds.material_level, 5) + 0.1*ob->bluerating );
    odds_num = static_cast<int>( 100*pow(odds.material_level+3, 3));

	return odds_num;

	FUNCTION_END;
}

/**
* \brief 得到打造金色装备的几率
* 
* 得到打造金色装备的几率
* 
* \param ob : 物品基本资料结构
* \return 得到打造金色装备的几率
*/
int CMakeData::odds_of_gold( const ObjectBase_t* ob)
{
	FUNCTION_BEGIN;

	/* zzf 2005.7.12
	return static_cast<int>( 0.65*pow(odds.material_level, 6) + 0.1*ob->goldrating);
	*/

	int odds_num =  ob->goldrating; 
	odds_num = static_cast<int>( 4*pow(odds.material_level, 7));

	return odds_num;

	FUNCTION_END;
}

/**
* \brief 得到打造神圣装备的几率
* 
* 得到打造神圣装备的几率
* 
* \param ob : 物品基本资料结构
* \return 得到打造神圣装备的几率
*/
int CMakeData::odds_of_holy( int object)
{
	FUNCTION_BEGIN;

	//return static_cast<int>( 50*odds.per/(odds.per+4) + 50*odds.luck/(odds.luck+10) + 0.09*odds.material_level*odds.material_level*odds.material_level*odds.material_level*odds.material_level + 10*odds.skill_level + 10*object);
	//return static_cast<int>( 0.1*pow(odds.material_level, 5) + object);
	//return static_cast<int>( 0.013*pow(odds.material_level, 7) + object);
	int odds_num = object;
	odds_num = static_cast<int>( 0.01*pow(odds.material_level, 10) );

	return odds_num;

	FUNCTION_END;
}

/**
 * \brief 得到装备附加属性的几率
 * 
 * 得到装备附加属性的几率
 * 
 * \param object : 物品属性数加增加的几率宝石
 * \param property : 从道具表中读到此物品的属性值
 * \return 得到装备附加属性的几率
 */
int CMakeData::odds_of_property(int object, int property)
{
	FUNCTION_BEGIN;

	//return static_cast<int>((3*odds.per/(odds.per+4) + 3*odds.luck/(odds.luck+10) + object )/property);
	//return static_cast<int>(  (  3*odds.per/(odds.per+4) + 3*odds.luck/(odds.luck+10) + odds.odds_gem + object + 5*odds.material_level )/property  );
	return static_cast<int>(  (  3*odds.per/(odds.per+4) + 3*odds.luck/(odds.luck+10) + odds.odds_gem + object + 5*odds.material_level )/property  );

	FUNCTION_END;
}


int CMakeData::level_of_material(DWORD id, DWORD num, WORD level,WORD needlevel,DWORD obid1,DWORD obid2,WORD level1,WORD level2)
{
	ObjectBase_t* ob = GetObjectBase(id);
	if (!ob) return false;
	
	float times=1.0f;
	if (ob->level>=2)
	{
		//if ((id==obid2)&&(level==level2))
			times=5.0f;
		sum_current += times * num * (level- needlevel +1);
		sum_base += times * num;

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("name:%s num:%d level:%d needlevel:%d current_level:%.10f  base:%.10f \n",
				ob->strName,num,level,needlevel,(float)times * num * (level- needlevel +1),(float)times * num);
#endif
	}
	else
	{
		//if ((id==obid1)&&(level==level1))
			times=5.0f;
		sum_current += 0.1 * times * num * (level- needlevel +1);
		sum_base += 0.1 * times * num;

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("name:%s num:%d level:%d needlevel:%d current_level:%.10f  base:%.10f \n",
			ob->strName,num,level,needlevel,(float)0.1 * times * num * (level- needlevel +1),(float)0.1 * times * num);
#endif
	}

	return true;
}

/**
 * \brief 根据玩家的幸运,魅力,打造技能等级设置打造几率.
 * 
 * 根据玩家的幸运,魅力,打造技能等级设置打造几率.
 * 
 * \param pUserData : 玩家结构
 * \param nSkillLevel : 打造技能等级
 * \param hole : 为0(未用)
 * \return 无
 */
void CMakeData::SetOdds(t_MainUserData* pUserData,int nSkillLevel,int hole)
{
	FUNCTION_BEGIN;

	sum_current = 0;
	sum_base = 0;

	DWORD obid1 = 0 ,level1 = 0 , obid2 = 0 ,level2 = 0;

	for (int i=0;i<m_SelectInfo.size();i++)
	{
		if (m_SelectInfo[i]->GetObjectBase()->level>=2)
		{
			if (m_SelectInfo[i]->nLevel>level2)
			{
				level2 = m_SelectInfo[i]->nLevel;
				obid2 = m_SelectInfo[i]->GetObjectBaseID();
			}
		}
		else
		{
			if (m_SelectInfo[i]->nLevel>level1)
			{
				level1 = m_SelectInfo[i]->nLevel;
				obid1 = m_SelectInfo[i]->GetObjectBaseID();
			}
		}
	}


	for (int i=0;i<m_SelectInfo.size();i++)
	{
		level_of_material(m_SelectInfo[i]->GetObjectBaseID(),
			m_SelectInfo[i]->nUseNum,m_SelectInfo[i]->nLevel,m_SelectInfo[i]->nNeedLevel,obid1,obid2,level1,level2);
	}

	material_level =  sum_base==0?0:sum_current / sum_base;

#if defined _DEBUG || defined RELEASE_TEST
	Engine_WriteLogF("material_level: %.10f \n",material_level);
#endif

	odds.luck = pUserData->lucky;
	odds.material_level=material_level;
	odds.per=pUserData->charm;
	odds.skill_level=nSkillLevel;
	if (hole>0)
		odds.odds_gem = 20;
	else
		odds.odds_gem = 0;


	FUNCTION_END;
}

/**
 * \brief 检查玩家的银子是否够
 * 
 * 检查玩家的银子是否够
 * 
 * \param nMoney : 要检查的银子数
 * \return 返回检查结果(fals为不够)
 */
bool CMakeData::CheckMoney(DWORD nMoney)
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	//Consider tax
	int nTax = (int)( CCountry::Instance()->GetTax() / 100.0f * nMoney + 0.5f );
	nMoney += nTax;

	if (pMainRole->GetMoney()>=nMoney)
		return true;
	else
		return false;

	FUNCTION_END;
}

/**
 * \brief 检查n大小的pPt数组中是否存在condition
 * 
 * 检查n大小的pPt数组中是否存在condition
 * 
 * \param condition : 被检查的数
 * \param n : 数组大小
 * \param pPt : 数组
 * \return 返回检查结果
 */
bool CMakeData::CheckCondition(stPointI condition,int n,stPointI* pPt)
{
	FUNCTION_BEGIN;
	
	bool bRet = true;

	for (int i=0;i<n;i++)
	{
		if (condition.x==pPt[i].x)
		{
			if (condition.y!=pPt[i].y)
			{
				bRet = false;
				break;
			}
		}
	}
	return bRet;

	FUNCTION_END;
}

/**
 * \brief n大小的pPt数组中是否存在x=pPt[].x,找到返回pPt[].y
 * 
 * n大小的pPt数组中是否存在x=pPt[].x,找到返回pPt[].y
 * 
 * \param x : 被检查的数
 * \param n : 数组大小
 * \param pPt : 数组
 * \return 返回值找到的pPt[].y
 */
int CMakeData::GetPt_y(int x,int n,stPointI* pPt)
{
	FUNCTION_BEGIN;

	int nRet =0 ;
	for (int i=0;i<n;i++)
	{
		if (x==pPt[i].x)
		{
			nRet=pPt[i].y;
			break;
		}
	}
	return nRet;

	FUNCTION_END;
}

/**
 * \brief 检查当前的材料是否还能合成
 * 
 * 检查当前的材料是否还能合成
 * 
 * \param dwCurrLevel : 当前材料的等级
 * \param dwMaxLevel : 此材料的最大等级
 * \param eState : 打造类型
 * \return 检查结果
 */
bool CMakeData::CheckChange(DWORD dwCurrLevel,DWORD dwMaxLevel,byte eState)
{
	FUNCTION_BEGIN;

	bool bChange = true;
	switch(eState) {
	case CGuiNpcDialog::eChange:
	case CGuiNpcDialog::eKaChange:
		if (dwCurrLevel==dwMaxLevel)
			bChange = false;
		break;
	}
	return bChange;

	FUNCTION_END;
}

/**
 * \brief 检查合成的物品是否足够
 * 
 * 检查合成的物品是否足够
 * 
 * \param dwID : 要检查的物品ID
 * \param num : 要检查的物品数量
 * \return 检查结果
 */
bool CMakeData::CheckItemNum(DWORD dwID,DWORD num)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		for (int i=0;i<=pObjectBase->dwChange;i++)
		{
			int nItemCount = GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
			if (nItemCount >= num)
			{
				return true;
			}
		}
	}

	return false;

	FUNCTION_END;
}


/**
* \brief 检查合成的物品是否足够
* 
* 检查合成的物品是否足够
* 
* \param dwID : 要检查的物品ID
* \param num : 要检查的物品数量
* \return 检查结果
*/
bool CMakeData::CheckItemNum(DWORD dwID,DWORD num,byte nLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,nLevel);
		if (nItemCount >= num)
		{
			return true;
		}
		
	}

	return false;

	FUNCTION_END;
}

//soke 改造
bool CMakeData::CheckModifyItemNum(DWORD dwID,DWORD num,byte nLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = 0;
		
		for (int i=nLevel;i<=pObjectBase->dwChange;i++)
		{
			nItemCount += GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
		}
		
		if (nItemCount >= num)
		{
			return true;
		}

	}
	return false;

	FUNCTION_END;
}

//soke 祝福改造
bool CMakeData::CheckMakezsItemNum(DWORD dwID,DWORD num,byte nLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = 0;
		
		for (int i=nLevel;i<=pObjectBase->dwChange;i++)
		{
			nItemCount += GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
		}
		
		if (nItemCount >= num)
		{
			return true;
		}

	}
	return false;

	FUNCTION_END;
}

//soke 马匹装备祝福改造
bool CMakeData::CheckMakezqItemNum(DWORD dwID,DWORD num,byte nLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = 0;
		
		for (int i=nLevel;i<=pObjectBase->dwChange;i++)
		{
			nItemCount += GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
		}
		
		if (nItemCount >= num)
		{
			return true;
		}

	}
	return false;

	FUNCTION_END;
}

//soke 升级改造
bool CMakeData::CheckMakelvItemNum(DWORD dwID,DWORD num,byte nLevel,bool bFixLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = 0;
		if(bFixLevel)
		{
			nItemCount = GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,nLevel);
		}
		else
		{
			for (int i=nLevel;i<=pObjectBase->dwChange;i++)
			{
				nItemCount += GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
			}
		}
		
		if (nItemCount >= num)
		{
			return true;
		}

	}

	return false;

	FUNCTION_END;
}

bool CMakeData::CheckMakeItemNum(DWORD dwID,DWORD num,byte nLevel)
{
	FUNCTION_BEGIN;

	ObjectBase_t* pObjectBase = GetObjectBase(dwID);

	if (pObjectBase)
	{
		int nItemCount = 0;
		
		for (int i=nLevel;i<=pObjectBase->dwChange;i++)
		{
			nItemCount += GetScene()->GetMainCharacter()->GetItemCount(dwID,OBJECTCELLTYPE_COMMON,i);
		}
		
		if (nItemCount >= num)
		{
			return true;
		}

	}

	return false;

	FUNCTION_END;
}
/**
 * \brief 获得物品的带孔数
 * 
 * 获得物品的带孔数
 * 
 * \param pSocket : 物品带孔结构
 * \param nUseSocket : 返回已经使用孔数
 * \return 获得物品的带孔数
 */
WORD CMakeData::CheckKaMake(DWORD* pSocket,WORD& nUseSocket)
{
	FUNCTION_BEGIN;

	WORD nSocket = 0;
	nUseSocket = 0;
	for (int i=0;i<SOCKETNUM;i++)
	{
		long socket=pSocket[i];
		switch(socket) {
		case -1:
			nSocket++;
			break;
		case 0:
			break;
		default:
			nUseSocket++;
			break;
		}
	}
	return nSocket;

	FUNCTION_END;
}

/**
 * \brief 得到物品名字和等级的字符串
 * 
 * 得到物品名字和等级的字符串
 * 
 * \param pDest : 要得到的物品名字和等级的字符串
 * \param pName : 物品的名字
 * \param nLevel : 物品的等级
 * \param type : 物品的类型
 * \return 无
 */
void CMakeData::GetNameAndLevel(char* pDest,const char* pName,int nLevel,int type,byte maxlevel)
{
	FUNCTION_BEGIN;

	char namePartOne[MAX_NAMESIZE];
	char namePartTwo[MAX_NAMESIZE];
	char* pStr = NULL;

	namePartOne[0] = 0L;
	namePartTwo[0] = 0L;

	pStr = strpbrk( pName, "0" );
	if ( pStr )
	{
		sprintf( namePartOne, ++pStr );
		int len = strlen(pName)-strlen(namePartOne)-1;
		strncpy( namePartTwo, pName, len );
		namePartTwo[len] = 0;
	}
	else
		sprintf( namePartOne, pName );


	if (maxlevel==0)
	{
		sprintf(pDest,"%s",namePartOne);
		return;
	}

	if (type<30)
	{
		if ( strlen(namePartOne) >= 8 )
			sprintf(pDest,"%s%s",namePartOne,g_szNumber[nLevel+1]);
		else
			sprintf(pDest,NAMEFORMAT2,namePartOne,g_szNumber[nLevel+1]);
	}
	else
	{
		if (nLevel!=0)
			sprintf(pDest,NAMEFORMAT,namePartOne,nLevel);
		else
			sprintf(pDest,"%s",namePartOne);
	}

	FUNCTION_END;
}


/**
 * \brief 得到拆解物品的银子
 * 
 * 得到拆解物品的银子
 * 
 * \param pRoleItem : 要拆解物品的结构
 * \return 得到拆解物品的银子
 */
int CMakeData::GetBackoutMoney(CRoleItem* pRoleItem)
{
	FUNCTION_BEGIN;
	if (!pRoleItem)
		return 0;

	int nMoney = pRoleItem->GetObjectBase()->needLevel/20 + 1;



	if (pRoleItem->GetObject()->kind&0x4)
	{
		nMoney *= 20;
	}
	else if (pRoleItem->GetObject()->kind&0x2)
	{
		nMoney *= 10;
	}
	else if (pRoleItem->GetObject()->kind&0x1)
	{
		nMoney *= 5;
	}
	else
		nMoney = 0;

	return nMoney;

	FUNCTION_END;
}

/**
 * \brief 检查是否能打造的技能等级
 * 
 * 检查是否能打造的技能等级
 * 
 * \param strSkillNeed : 技能等级需求字符串
 * \param szSkillShow : 返回技能等级说明字符串
 * \return 检查结果
 */
bool CMakeData::IsCanSkill(char* strSkillNeed,char** szSkillShow)
{
	FUNCTION_BEGIN;

	static char szSkillNeed[MAX_NAMESIZE];
	std::vector<UINT, std::stack_allocator<UINT> > aid;

	aid.reserve(4);
	_parse_str_num(aid,strSkillNeed);
	
	(*szSkillShow) = szSkillNeed;
	szSkillNeed[0]=0;
	if (aid.size()>0)
	{
		CRoleSkill* pSkill=GetScene()->GetMainCharacter()->FindSkillByID(aid[0]);
		SkillBase_t* pSkillBase = GetSkillBase(aid[0]);
		
		if (pSkillBase)
			sprintf(szSkillNeed,"%s+%d",pSkillBase->pstrName,aid[1]<1?1:aid[1]);

		if (aid[1]>1)
		{
			if (!pSkill)
				return false;
			else if (pSkill->GetLevel()<aid[1])
				return false;
		}
	}

	return true;

	FUNCTION_END;
}

/**
 * \brief 根据打造需求字符串检查是否可以打造
 * 
 * 根据打造需求字符串检查是否可以打造
 * 
 * \param strItemNeed : 打造需求字符串
 * \return 返回检查结果
 */
bool CMakeData::IsMakeItem(char* strItemNeed,DWORD dwLevel)
{
	FUNCTION_BEGIN;

	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);

	_parse_str_num(aid,strItemNeed);

	
	if (aid.size()>0)
	{
		if (!CheckMoney(aid[0]))
			return false;

		//soke 去掉打造功勋
		/*if (dwLevel>=80)
		{
			if (GetScene()->GetMainCharacter()->GetProData()->exploit<EXPFORMAKE)
				return false;
		}*/

		for (int i=1;i<aid.size();i+=3)
		{
			if (!CheckMakeItemNum(aid[i],aid[i+2],aid[i+1]))
				return false;
		}
	}
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 根据改造需求字符串检查是否可以改造
 * 
 * 根据改造需求字符串检查是否可以改造
 * 
 * \param strItemNeed : 改造需求字符串
 * \return 返回检查结果
 */
bool CMakeData::IsCanModify(char* strItemNeed,DWORD dwLevel)
{
FUNCTION_BEGIN;

	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);

	_parse_str_num(aid,strItemNeed);

	
	if (aid.size()>0)
	{
		//soke 判断改造需要的银子
		if (!CheckMoney(11000))
			return false;
		//548 就是改造材料
		if (!CheckModifyItemNum(548,2,dwLevel))
			return false;
		//for (int i=1;i<aid.size();i+=3)
		//{
		//	//改造只需要高级材料  数目==2
		//	if( aid[i] == 507||
		//		aid[i] == 517||
		//		aid[i] == 527||
		//		aid[i] == 537||
		//		aid[i] == 547||
		//		aid[i] == 548)
		//	{
		//		if (!CheckModifyItemNum(548,2,dwLevel))
		//			return false;
		//	}
		//	else
		//	{
		//		continue;
		//	}
		//}
	}
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 根据改造需求字符串检查是否可以祝福改造
 * 
 * 根据改造需求字符串检查是否可以改造
 * 
 * \param strItemNeed : 改造需求字符串
 * \return 返回检查结果
 */
bool CMakeData::IsCanMakezs(char* strItemNeed,DWORD dwLevel)
{
FUNCTION_BEGIN;

	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);

	_parse_str_num(aid,strItemNeed);

	
	if (aid.size()>0)
	{
		//soke 判断神圣祝福需要的银子
		if (!CheckMoney(500001))
			return false;
		//1317 就是改造材料
		if (!CheckMakezsItemNum(1317,5,dwLevel))
			return false;
		//for (int i=1;i<aid.size();i+=3)
		//{
		//	//改造只需要高级材料  数目==2
		//	if( aid[i] == 507||
		//		aid[i] == 517||
		//		aid[i] == 527||
		//		aid[i] == 537||
		//		aid[i] == 547||
		//		aid[i] == 548)
		//	{
		//		if (!CheckModifyItemNum(548,2,dwLevel))
		//			return false;
		//	}
		//	else
		//	{
		//		continue;
		//	}
		//}
	}
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 根据改造需求字符串检查是否可以祝福改造
 * 
 * 根据改造需求字符串检查是否可以改造
 * 
 * \param strItemNeed : 改造需求字符串
 * \return 返回检查结果
 */
bool CMakeData::IsCanMakezq(char* strItemNeed,DWORD dwLevel)
{
FUNCTION_BEGIN;

	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);

	_parse_str_num(aid,strItemNeed);

	
	if (aid.size()>0)
	{
		//soke 判断神圣祝福需要的银子
		if (!CheckMoney(500001))
			return false;
		//1376 就是改造材料
		if (!CheckMakezqItemNum(1376,5,dwLevel))
			return false;
		//for (int i=1;i<aid.size();i+=3)
		//{
		//	//改造只需要高级材料  数目==2
		//	if( aid[i] == 507||
		//		aid[i] == 517||
		//		aid[i] == 527||
		//		aid[i] == 537||
		//		aid[i] == 547||
		//		aid[i] == 548)
		//	{
		//		if (!CheckModifyItemNum(548,2,dwLevel))
		//			return false;
		//	}
		//	else
		//	{
		//		continue;
		//	}
		//}
	}
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 根据改造需求字符串检查是否可以升级改造
 * 
 * 根据改造需求字符串检查是否可以改造
 * 
 * \param strItemNeed : 改造需求字符串
 * \return 返回检查结果
 */
bool CMakeData::IsCanMakelv(char* strItemNeed,DWORD dwLevel,DWORD needLevel)
{
	FUNCTION_BEGIN;

	std::vector<UINT, std::stack_allocator<UINT> > aid;
	aid.reserve(16);

	_parse_str_num(aid,strItemNeed);

	
	if (aid.size()>0)
	{
		//soke 判断装备升级需要的银子
		if (!CheckMoney(400001))
			return false;
		//550 就是升级材料
		if (!CheckMakelvItemNum(MAKELVTONE,MAKELVNUM,dwLevel,true))
			return false;

		if(needLevel > 160)
		{
			int index = aid.size() - 3;
			if (!CheckMakelvItemNum(aid[index],aid[index+2],aid[index+1],true))
				return false;
		}
		
	}
	
	return true;

	FUNCTION_END;
}

/**
 * \brief 根据物品结构检查是否可以升级
 * 
 * 根据物品结构检查是否可以升级
 * 
 * \param pData : 物品结构
 * \return 返回检查结果
 */
bool  CMakeData::IsUpItem(stPropInfo* pData)
{
	FUNCTION_BEGIN;

	bool bRet = false;
	
	if (pData)
	{
		DWORD  dwUpID=(pData->nLevel+1)*10000+ ((CRoleItem*)(pData->pObject))->GetObjectBase()->dwID;
		ObjectUp_t* pUp=GetObjectUp(dwUpID);
		if(pUp)
		{
			if (CheckMoney(pUp->dwMoney))
			{
				//if (GetScene()->GetMainCharacter()->GetItemCount(UPOBJCET,0,pData->nLevel) > 0)
				if (GetScene()->GetMainCharacter()->GetItemCount(UPOBJCET,OBJECTCELLTYPE_COMMON) > 0)
				{
					bRet = true;
				}
			}
		}

	}

	return bRet;

	FUNCTION_END;
}

/**
* \brief 根据物品结构检查是否可以打孔
* 
* 根据物品结构检查是否可以打孔
* 
* \param pData : 物品结构
* \return 返回检查结果
*/
bool  CMakeData::IsHoleMake(stPropInfo* pData)
{
	FUNCTION_BEGIN;

	bool bRet = false;

	if (pData)
	{
		int nSocket=pData->nUseSocket+pData->nCanSocket;
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = GetInfo(GetMainItem(CGuiNpcDialog::eHoleMake,ePrice),
			nSocket+1,"32000");

		if (CheckMoney(ainfo[0]))
		{
			if (GetScene()->GetMainCharacter()->GetItemCount(HOLESTONE,OBJECTCELLTYPE_COMMON,nSocket) > 0)
			{
				bRet = true;
			}
		}
	}

	return bRet;

	FUNCTION_END;
}

/**
 * \brief 根据物品结构检查是否可以合成
 * 
 * 根据物品结构检查是否可以合成
 * 
 * \param pData : 物品结构
 * \param nAdd : 是否可以不同等级合成
 * \return 检查结果
 */
bool CMakeData::IsChangeItem(stPropInfo* pData,int nAdd)
{
	FUNCTION_BEGIN;

	bool bRet = false;

	if (pData)
	{
		if (CheckMoney(pData->GetObjectBase()->dwChangePrice))
		{
			if (GetScene()->GetMainCharacter()->GetItemCount(pData->ObjectID,OBJECTCELLTYPE_COMMON,pData->nLevel-1) >= nAdd)
			{
				bRet = true;
			}
		}
	}

	return bRet;

	FUNCTION_END;
}

/**
 * \brief 根据物品结构检查是否可以镶嵌
 * 
 * 根据物品结构检查是否可以镶嵌
 * 
 * \param pData : 物品结构
 * \return 检查结果
 */
bool CMakeData::IsKaMake(stPropInfo* pData)
{
	FUNCTION_BEGIN;

	bool bRet = false;

	if (pData)
	{
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = GetInfo(GetMainItem(CGuiNpcDialog::eKaMake,ePrice),
			pData->nUseSocket+1,"16000");

		
		if (CheckMoney(ainfo[0]))
		{
			if (GetScene()->GetMainCharacter()->GetItemCount(MAKESTONE,OBJECTCELLTYPE_COMMON,pData->nUseSocket) > 0)
			{
				if (GetScene()->GetMainCharacter()->GetItemCount(KASTONE,OBJECTCELLTYPE_COMMON) > 0)
				{
					bRet = true;
				}
			}
		}

	}

	return bRet;

	FUNCTION_END;
}

/**
 * \brief 根据魂魄结构检查是否可以合成
 * 
 * 根据魂魄结构检查是否可以合成
 * 
 * \param pData : 魂魄结构
 * \return 检查结果
 */
bool CMakeData::IsKaChange(stPropInfo* pData)
{
	FUNCTION_BEGIN;

	bool bRet = false;

	if (pData)
	{
		if (pData->ObjectID==KASTONE)
		{
			if (CheckMoney(pData->GetObjectBase()->dwChangePrice))
			{
				if (GetScene()->GetMainCharacter()->GetItemCount(RATESTONE,OBJECTCELLTYPE_COMMON,pData->nLevel-1) > 0)
				{
					int nCount = GetScene()->GetMainCharacter()->GetItemCount(KASTONE,OBJECTCELLTYPE_COMMON,pData->nLevel-1);

					if (nCount > 1)
					{
						bRet = true;
					}
					else if (nCount == 1)
					{
						for (int i=0;i<pData->nLevel-1;i++)
						{
							if (GetScene()->GetMainCharacter()->GetItemCount(KASTONE,OBJECTCELLTYPE_COMMON,i)>0)
							{
								bRet = true;
								break;
							}
						}
					}

				}
			}
		}

	}

	return bRet;

	FUNCTION_END;
}

/**
 * \brief 打造,合成,升级等对话框构造函数
 * 
 * 打造,合成,升级等对话框构造函数
 * 
 * \return 无
 */
CGuiNpcMake::CGuiNpcMake(byte eState)
:m_pMaterial(NULL)
,m_pMake(NULL)
,m_pClew(NULL)
,m_pNeed(NULL)
,m_pProcess(NULL)
,m_bPass(false)
,m_bMoneyOk(false)
,m_pMakeGuess(NULL)
,m_bProcess(false)
,m_bEnableMake(false)
,m_dwChangeID(0)
,m_bShowExpTitle(false)
{
	FUNCTION_BEGIN;

	m_eState = eState;

	FUNCTION_END;
}


/**
 * \brief 加载打造数据
 * 
 * 加载打造数据
 * 
 * \param params : 从服务器传过来的打造字符串
 * \param bReLoad : 是否是重新加载数据
 * \param dwChangeID : 合成道具id
 * \return 无
 */
void CGuiNpcMake::LoadData(std::vector<std::string> & params,bool bReLoad,DWORD dwChangeID)
{
	FUNCTION_BEGIN;

	m_bProcess = false;

	if (!bReLoad)
	{
		m_dwChangeID = dwChangeID;
		m_params.clear();
		for (int i=0;i<params.size();i++)
			m_params.push_back(params[i]);

		GetStatic(8)->SetText(params[1].c_str());
	}


	SetMakeToParam(m_params);
		
	if (m_pProcess)
		m_pProcess->SetPos(0);

	UpdateInfo();

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiNpcMake::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiNpcMake = NULL;

	if (GetGameGuiManager()->m_guiMakeGuess)
	{
		GetGameGuiManager()->m_guiMakeGuess->SetVisible(false);
	}

	if((GetGameGuiManager()->m_guiNpcDialog)&&(m_eState!=CGuiNpcDialog::eChange))
	{
		GetGameState()->SetStateItem(eGameStateItem_NpcDialog);
		GetGameGuiManager()->OnNpcActionEnd();
	}
	else
		GetGameState()->SetStateItem(eGameStateItem_Normal);

	FUNCTION_END;
}

/**
 * \brief 对话框创建
 * 
 * 对话框创建
 * 
 * \return 无
 */
void CGuiNpcMake::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	
	m_pClew = GetStatic(ClewLabel);
	m_pNeed = GetStatic(NeedLabel);
	m_pMake = GetButton(MakeButton);
	m_pMaterial = GetListBox(MaterialListBox);
	m_pProp = GetListBox(PropListBox);
	m_pProcess = GetProcess(MakeProcess);
	
	m_pClew->SetText("0");
	m_pClew->SetVisible(false);
	m_pNeed->SetText("");
	
	m_pMake->SetEnabled(false);
	
	m_BackElements.resize(1);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = YELLOWCOLOR;
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_BOTTOM | DT_CENTER;
	m_BackElements[0] = pEmt;

	if (GetButton(ChangeNum))
	{
		GetButton(ChangeNum)->GetToolTips()->SetBkColor(COLOR_ARGB(128,0,0,0));
		GetButton(ChangeNum)->GetToolTips()->SetCurColor(COLOR_ARGB(255,255,255,255));
		GetButton(ChangeNum)->GetToolTips()->Clear();
		GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数5\n合成成功率100%");
		GetButton(ChangeNum)->GetToolTips()->Resize();
	}

	m_pMake->GetToolTips()->SetBkColor(COLOR_ARGB(128,0,0,0));
	m_pMake->GetToolTips()->SetCurColor(COLOR_ARGB(255,255,255,255));
	m_pMake->GetToolTips()->Clear();

	CreateButtons();
	
	if (GetButton(EnableMakeButton)!=NULL)
		OnGuiEvent(EVENT_BUTTON_CLICKED,EnableMakeButton,NULL);

	FUNCTION_END;
}

/**
 * \brief 设置关闭按钮位置
 * 
 * 设置关闭按钮位置
 * 
 * \return 无
 */
void CGuiNpcMake::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y+26);
	}

	FUNCTION_END;
}

/**
 * \brief 设置对话框皮肤
 * 
 * 设置对话框皮肤
 * 
 * \return 无
 */
void CGuiNpcMake::InitCustomElements()
{
	FUNCTION_BEGIN;

	
	

	FUNCTION_END;
}

/**
 * \brief 刷新对话框数据，图片
 * 
 * 刷新对话框数据，图片
 * 
 * \param fElapsedTime : 刷新时间
 * \return 刷新结果
 */
HRESULT CGuiNpcMake::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	static stPointI ptOld(m_pMaterial->GetFirstShowItem(),m_pMaterial->GetLastShowItem());
	static float m_fRunTime=0.0f;
	
	stPointI ptNew(m_pMaterial->GetFirstShowItem(),m_pMaterial->GetLastShowItem());
	if (ptOld!=ptNew)
	{
		OnGuiEvent(EVENT_LISTBOX_CHANGED,MaterialListBox,NULL);
	}
	ptOld=ptNew;

	//if (m_pMakeGuess)
	{
		if ((m_bProcess)&&(m_pProcess))
		{
			m_fRunTime+=fElapsedTime;
			if (m_fRunTime>0.05f) //myy 打造进度条时间
			{
				m_fRunTime=0.0f;
				if (m_pProcess->GetPos()>=100)
				{
					OnGuiEvent(EVENT_BUTTON_CLICKED,ProcessFinish,NULL);//m_pProcess->SetPos(0);
					//m_aniMake.SetFrame(m_aniMake.GetFrameCount()-1);
					m_aniMake.SetLoopPlay(false);
				}
				else
					m_pProcess->SetPos(m_pProcess->GetPos()+3);
			}
		}
	}
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	int nMoney = strtol(m_pClew->GetText(),NULL,10);
	if (m_MakeData.CheckMoney(nMoney)||(m_eState==CGuiNpcDialog::eBackout))
		m_MoneyColor = GREENCOLOR;
	else	
		m_MoneyColor = REDCOLOR;

	//Add tax
	nMoney += (int)( CCountry::Instance()->GetTax() / 100.0f * nMoney + 0.5f );

	m_BackElements[0]->FontColor.Current =  m_MoneyColor;
	DrawText(Money2String(nMoney),m_BackElements[0],&(stRectI(280,517,365,529)));		//atoi(m_pClew->GetText())
	
	//soke 去掉打造需要功勋显示
	/*if (m_bShowExpTitle)
	{
		DWORD  expColor = GREENCOLOR;
		if (GetScene()->GetMainCharacter()->GetProData()->exploit<EXPFORMAKE)
		{
			expColor = REDCOLOR;
		}
		m_BackElements[0]->FontColor.Current =  expColor;
		DrawText("需要5点功勋值",m_BackElements[0],&(stRectI(120,150,220,166)));
	}*/

	m_aniMake.Render(m_x,m_y);


	stPointI ptTest(Engine_GetCursor()->GetPosition());
	//stPointI pt = ptTest + stPointI(28,28);
	stRectI  rect;

	ptTest -= stPointI(GetClientX(),GetClientY());
	
	int index=-1;
	if (m_pProp)
	{
		index=m_pProp->GetMoveSel();
		
		rect=m_pProp->GetMoveSelRect();
		rect.OffsetRect(GetClientX(),GetClientY());

		if ((index>=0)&&(index<m_pProp->GetCount())&&(m_pProp->OnHiTest(ptTest)!=HTERROR))
		{
			stPropInfo* pData=(stPropInfo*)m_pProp->GetItemData(index);
			if (pData)
			{
				switch(m_eState) {
				case CGuiNpcDialog::eKaMake:
					if (pData->pObject)
					{
						CRoleItem* pObject = (CRoleItem*)pData->pObject;
						if (m_bPass&&(index==m_pProp->GetCurItem()))
						{
							
							UpdateMyObjectToolTips(m_Object,pObject->GetObjectBase(),m_ToolTips,0);
							m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());

							//m_ToolTips.Render(pt.x,pt.y);
						}
						else
							pObject->ShowToolTipsTopLeftPrefer(rect);
					}
					break;
				case CGuiNpcDialog::eKaChange:
					if (pData->pObject)
					{
						
						if (m_bPass&&(index==m_pProp->GetCurItem()))
						{
							UpdateMyObjectToolTips(m_Object,(ObjectBase_t*)pData->pObject,m_ToolTips,0);
							
						}
						else
						{
							UpdateObjectToolTips((ObjectBase_t*)pData->pObject,m_ToolTips,0,1);
							
							
						}
						
						m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());

					}
					break;
				default:
					if (pData->bObject)
					{
						UpdateObjectToolTips((ObjectBase_t*)pData->pObject,m_ToolTips,0,1);

						

						m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());

					}
					else
					{
						((CRoleItem*)pData->pObject)->ShowToolTipsTopLeftPrefer(rect);
					}

				}
			}
		}
	}
	
	if (m_pMaterial)
	{
		index=m_pMaterial->GetMoveSel();
		if ((index>=0)&&(index<m_pMaterial->GetCount())&&(m_pMaterial->OnHiTest(ptTest)!=HTERROR))
		{
			stMaterialInfo* pData=(stMaterialInfo*)m_pMaterial->GetItemData(index);
			if (pData)
			{
				rect=m_pMaterial->GetMoveSelRect();
				rect.OffsetRect(GetClientX(),GetClientY());

				if (pData->bAdd)
				{
					UpdateObjectToolTips((ObjectBase_t*)pData->pObject,m_ToolTips,0,1);

					
					m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());

				}
				else
				{
					((CRoleItem*)pData->pObject)->ShowToolTipsTopLeftPrefer(rect);
				}
			}
		}
	}
	
	return hr;

	FUNCTION_END;
}

/**
 * \brief 通知服务器转盘结束
 * 
 * 通知服务器转盘结束
 * 
 * \return 无
 */
void CGuiNpcMake::SendGuessFinish()
{
	FUNCTION_BEGIN;

	stFoundItemPropertyUserCmd cmd;
	cmd.up_id=0;

	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 根据打造结果设置转盘结束的角度
 * 
 * 根据打造结果设置转盘结束的角度
 * 
 * \param pCmd : 打造结果
 * \return 无
 */
void CGuiNpcMake::DisMake(stFoundItemPropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	if (m_pMakeGuess)
	{
		m_pMakeGuess->SetEndAngle(pCmd->up_id);
	}

	FUNCTION_END;
}

/**
 * \brief 根据返回结果重新进行下一次打造数据加载
 * 
 * 根据返回结果重新进行下一次打造数据加载
 * 
 * \param pCmd : 打造结果
 * \return 无
 */
void CGuiNpcMake::Response(stResponsePropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	if (IsEnabled()==false)
	{
		switch(pCmd->status) 
		{
		case 0:
		case 2:
		case 3:
			{
				PlayGameSound(SUCCESSSOUND,SoundType_UI);
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 100;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10010);
			}
			break;
		case 1:
			{
				PlayGameSound(FAILEDSOUND,SoundType_UI);
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 104;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10013);
			}
			break;
		case 4:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 102;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10011);
			}
			break;
		case 5:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 101;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10011);
			}
			break;
		}
		
		m_pMakeGuess=NULL;
		SetEnabled(true);
		LoadData(m_params,true);
	}

	FUNCTION_END;
}

/**
 * \brief 根据返回结果重新进行下一次改造数据加载
 * 
 * 根据返回结果重新进行下一次改造数据加载
 * 
 * \param pCmd : 改造结果
 * \return 无
 */
//soke 增加改造的进行中和完成的特效
void CGuiNpcMake::Response1(stResponsePropertyUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	if (IsEnabled()==false)
	{
		switch(pCmd->status) 
		{
		case 0:
		case 2:
		case 3:
			{
				PlayGameSound(SUCCESSSOUND,SoundType_UI);
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 101; //soke 改造完成显示的特效
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10011);
			}
			break;
		case 1:
			{
				PlayGameSound(FAILEDSOUND,SoundType_UI);
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 104;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10013);
			}
			break;
		case 4:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 102;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10011);
			}
			break;
		case 5:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 19;
				rl.frame = 101;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10011);
			}
			break;
		}
		
		m_pMakeGuess=NULL;
		SetEnabled(true);
		LoadData(m_params,true);
	}

	FUNCTION_END;
}

/**
 * \brief 发送打造数据
 * 
 * 发送打造数据
 * 
 * \return 发送成功结果
 */
bool CGuiNpcMake::SendCmd()
{
	FUNCTION_BEGIN;

	switch(m_eState) {
	//case CGuiNpcDialog::eChange:
	//	{
	//		stComposeItemPropertyUserCmd cmd;
	//		
	//		if (m_MakeData.getSelectData().size()==1)
	//		{
	//			stMaterialInfo* p=m_MakeData.getSelectData()[0];
	//			if (p)
	//			{
	//				cmd.gem_id=p->GetObjectBaseID();
	//				cmd.gem_level=p->nLevel;
	//				cmd.gem_num=p->nUseNum;
	//				cmd.gem_per_num=GetChangeNumBySingle();
	//				cmd.tool_id=m_dwChangeID;

	//				SEND_USER_CMD(cmd);
	//				return true;
	//			}
	//		}
	//	}
	//	break;
	//case CGuiNpcDialog::eKaChange:
	//	{
	//		stComposeSoulStonePropertyUserCmd cmd;

	//		if (m_MakeData.getSelectData().size()==3)
	//		{
	//			int n=0;
	//			for (int i=0;i<3;i++)
	//			{
	//				stMaterialInfo* p=m_MakeData.getSelectData()[i];
	//				if (!p)
	//					return false;
	//				
	//				if (p->GetObjectBaseID()==KASTONE)
	//				{
	//					if (n==0)
	//						cmd.id_1=p->ObjectID;
	//					else
	//						cmd.id_2=p->ObjectID;

	//					n++;
	//				}
	//				else
	//				{
	//					if (p->GetObjectBaseID()==RATESTONE)
	//					{
	//						cmd.gem_id=p->GetObjectBaseID();
	//						cmd.gem_level=p->nLevel;
	//						cmd.gem_num=p->nUseNum;
	//					}
	//					else
	//						return false;
	//				}

	//			}

	//			SEND_USER_CMD(cmd);
	//			return true;
	//		}
	//	}
	//	break;
	case CGuiNpcDialog::eMakeFood:
	case CGuiNpcDialog::eMakeDrug:
	case CGuiNpcDialog::eMake:
		{
			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			
			if (pData)
			{
				if (!m_bProcess)
				{
					{
						stResourceLocation rl;
						rl.SetFileName("data\\interfaces.gl");

						rl.group = 19;
						rl.frame = 103;
						m_aniMake.Create(&rl);
						m_aniMake.SetLoopPlay(true);

						PlayUISound(10012);
					}

					m_bProcess=true;
					return true;
				}
				
				m_bProcess = false;
				void* pTmp=NULL;
				int nLen=sizeof(stFoundItemPropertyUserCmd)+sizeof(stFoundItemPropertyUserCmd::material)*m_MakeData.getSelectData().size();
				pTmp=malloc(nLen);
				stFoundItemPropertyUserCmd* pCmd=(stFoundItemPropertyUserCmd*)pTmp;
				pCmd->byCmd = PROPERTY_USERCMD;
				pCmd->byParam = FOUNDITEM_PROPERTY_USERCMD_PARA;
				pCmd->up_id=pData->ObjectID;
				pCmd->count=m_MakeData.getSelectData().size();

				for (int i=0;i<pCmd->count;i++)
				{
					stMaterialInfo* p=m_MakeData.getSelectData()[i];
					if (!p)
					{
						free(pTmp);
						return false;
					}

					pCmd->list[i].gem_id=p->GetObjectBaseID();
					pCmd->list[i].gem_level=p->nLevel;
					pCmd->list[i].gem_num=p->nUseNum;
				}

				Client_SendCommand(pCmd,nLen,false);
				free(pTmp);

				
				if ((pData->bArm)&&(m_pMakeGuess)&&(m_pProcess))
				{
					m_pMakeGuess->m_bStart=true;
					m_bProcess=false;

					if (m_pProcess->GetPos()<33)
						m_pMakeGuess->SetSpeed(eSlow);
					else if (m_pProcess->GetPos()<66)
						m_pMakeGuess->SetSpeed(eMidd);
					else
						m_pMakeGuess->SetSpeed(eFast);

					m_pMakeGuess->SetVisible(true);
					m_pMakeGuess->SetEndAngle(0);	
				}
				
				return true;
			}
		}
		break;
	/*case CGuiNpcDialog::eUp:
		{
			stUpgradeItemPropertyUserCmd cmd;

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(UPOBJCET);
			
			if (pData&&p)
			{
				cmd.up_id=pData->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;
				p=m_MakeData.findSelectDataByObjectID(RATEOBJECT);
				if (p)
				{
					cmd.odds_id=p->GetObjectBaseID();
					cmd.odds_level=p->nLevel;
					cmd.odds_num=p->nUseNum;
				}
				else
				{
					cmd.odds_id=0;
					cmd.odds_level=0;
					cmd.odds_num=0;
				}

				SEND_USER_CMD(cmd);
				return true;
			}
		}
		break;
	case CGuiNpcDialog::eKaMake:
		{
			stEnchasePropertyUserCmd cmd;

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(MAKESTONE);
			stMaterialInfo* p2=m_MakeData.findSelectDataByObjectID(KASTONE);
			if (pData&&p&&p2)
			{
				cmd.up_id=pData->ObjectID;
				cmd.soulstone_id=p2->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;

				SEND_USER_CMD(cmd);
				return true;
			}
		}
		break;*/
	case CGuiNpcDialog::eHoleMake:
		{
			stHolePropertyUserCmd cmd;

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(HOLESTONE);

			if (pData&&p)
			{
				cmd.up_id=pData->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;

				SEND_USER_CMD(cmd);
				return true;
			}
		}
		break;
	case CGuiNpcDialog::eBackout:
		{
			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

			if (pData)
			{
				CRoleItem * pItem = GetScene()->GetMainCharacter()->FindItemByID(pData->ObjectID);
				assert(pItem);

				if( pItem && pItem->IsBind() && CSafetyUser::Instance()->IsSafeDecomposeObject())
				{
					GetGameGuiManager()->AddClientSystemMessage("你处于物品分割保护状态，不能分割绑定物品！");
					return false;
				}				

				if (!m_bProcess)
				{
					m_bProcess=true;
					return true;
				}

				m_bProcess = false;
				
				
				stDecomposePropertyUserCmd cmd;
				cmd.up_id = pData->ObjectID;
				SEND_USER_CMD(cmd);

				return true;
			}
		}
		break;
		
	//soke 改造装备
	case CGuiNpcDialog::eModifyAttri:           // 改造属性	
	case CGuiNpcDialog::eAddReBind:			    // 重新绑定
	case CGuiNpcDialog::eAddReMaker:			// 修改签名
	case CGuiNpcDialog::eAddAddMaker:			// 装备签名
	case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
	case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造
	case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
	case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
	case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
	case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
		{
			stModifyPropertyUserCmd cmd;
			if(m_eState == CGuiNpcDialog::eModifyAttri)
			{
				cmd.type = 0;
			}
			else if(m_eState == CGuiNpcDialog::eAddReBind)
			{
				cmd.type = 1;
			}
			else if(m_eState == CGuiNpcDialog::eAddReMaker)
			{
				cmd.type = 2;
			}
			else if(m_eState == CGuiNpcDialog::eAddAddMaker)
			{
				cmd.type = 3;
			}
			else if(m_eState == CGuiNpcDialog::eAddModifySoulA)
			{
				cmd.type = 4;
			}
			else if(m_eState == CGuiNpcDialog::eAddLVSoullineMD)
			{
				cmd.type = 5;
			}
			else if(m_eState == CGuiNpcDialog::eAddModifyDxSoulA)
			{
				cmd.type = 6;
			}
			else if(m_eState == CGuiNpcDialog::eAddMDSkillA)
			{
				cmd.type = 7;
			}
			else if(m_eState == CGuiNpcDialog::eAddEquipRound)
			{
				cmd.type = 8;
			}
			else if(m_eState == CGuiNpcDialog::eAddMDMake)
			{
				cmd.type = 9;
			}

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			// 增加改造进度条
			if (pData)
			{
				if (!m_bProcess)
				{
					{
						stResourceLocation rl;
						rl.SetFileName("data\\interfaces.gl");

						rl.group = 19;
						rl.frame = 103;
						m_aniMake.Create(&rl);
						m_aniMake.SetLoopPlay(true);

						PlayUISound(10012);
					}

					m_bProcess=true;
					return true;
				}
			}

			m_bProcess = false;
			// 改造进度条结束
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(MODIFYSTONE);

			if (pData&&p)
			{
				cmd.up_id=pData->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;

				SEND_USER_CMD(cmd);
				return true;
			}
	}
	break;
	case CGuiNpcDialog::eAddEquipMake:			// 装备祝福
		{
			stMakezsPropertyUserCmd cmd;
			if(m_eState == CGuiNpcDialog::eAddEquipMake)
			{
				cmd.type = 1;
			}

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			// 增加改造进度条
			if (pData)
			{
				if (!m_bProcess)
				{
					{
						stResourceLocation rl;
						rl.SetFileName("data\\interfaces.gl");

						rl.group = 19;
						rl.frame = 103;
						m_aniMake.Create(&rl);
						m_aniMake.SetLoopPlay(true);

						PlayUISound(10012);
					}

					m_bProcess=true;
					return true;
				}
			}

			m_bProcess = false;
			// 改造进度条结束
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(MAKEZSTONE);

			if (pData&&p)
			{
				cmd.up_id=pData->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;

				SEND_USER_CMD(cmd);
				return true;
			}
	}
	break;
	case CGuiNpcDialog::eAddEquipZsMake:			// 马匹装备祝福
		{
			stMakezqPropertyUserCmd cmd;
			if(m_eState == CGuiNpcDialog::eAddEquipZsMake)
			{
				cmd.type = 1;
			}

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			// 增加马匹装备改造进度条
			if (pData)
			{
				if (!m_bProcess)
				{
					{
						stResourceLocation rl;
						rl.SetFileName("data\\interfaces.gl");

						rl.group = 19;
						rl.frame = 103;
						m_aniMake.Create(&rl);
						m_aniMake.SetLoopPlay(true);

						PlayUISound(10012);
					}

					m_bProcess=true;
					return true;
				}
			}

			m_bProcess = false;
			// 改造进度条结束
			stMaterialInfo* p=m_MakeData.findSelectDataByObjectID(MAKEZQTONE);

			if (pData&&p)
			{
				cmd.up_id=pData->ObjectID;
				cmd.gem_id=p->GetObjectBaseID();
				cmd.gem_level=p->nLevel;
				cmd.gem_num=p->nUseNum;

				SEND_USER_CMD(cmd);
				return true;
			}
	}
	break;
	case CGuiNpcDialog::eAddEquipMaklv:			// 装备升级
		{

			stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
			// 增加改造进度条
			if (pData)
			{
				if (!m_bProcess)
				{
					{
						stResourceLocation rl;
						rl.SetFileName("data\\interfaces.gl");

						rl.group = 19;
						rl.frame = 103;
						m_aniMake.Create(&rl);
						m_aniMake.SetLoopPlay(true);

						PlayUISound(10012);
					}

					m_bProcess=true;
					return true;
				}
			}

			m_bProcess = false;
			// 改造进度条结束
			void* pTmp=NULL;
			int nLen=sizeof(stFoundItemPropertyUserCmd)+
				sizeof(stFoundItemPropertyUserCmd::material)*m_MakeData.getSelectData().size();
			pTmp=malloc(nLen);
			stFoundItemPropertyUserCmd* pCmd=(stFoundItemPropertyUserCmd*)pTmp;
			pCmd->byCmd = PROPERTY_USERCMD;
			pCmd->byParam = MAKELV_PROPERTY_USERCMD_PARA;
			pCmd->up_id=pData->ObjectID;
			pCmd->count=m_MakeData.getSelectData().size();

			for (int i=0;i<pCmd->count;i++)
			{
				stMaterialInfo* p=m_MakeData.getSelectData()[i];
				if (!p)
				{
					free(pTmp);
					return false;
				}

				pCmd->list[i].gem_id=p->GetObjectBaseID();
				pCmd->list[i].gem_level=p->nLevel;
				pCmd->list[i].gem_num=p->nUseNum;
			}

			Client_SendCommand(pCmd,nLen,false);
			free(pTmp);
	}
	break;

	}

	return false;


	FUNCTION_END;
}

/**
 * \brief 消息处理函数
 * 
 * 消息处理函数
 * 
 * \param nEvent : 消息事件
 * \param nID : 触发消息的控件ＩＤ
 * \param pControl : 触发消息的控件指针
 * \return 消息处理结果
 */
bool CGuiNpcMake::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	
	switch(nEvent)
	{
	case EVENT_LBUTTON_DOWN:
		{
			/*
			if ((nID==MakeButton)&&(m_eState==CGuiNpcDialog::eMake)&&m_pMake->IsEnabled())
			{
				stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

				if (pData)
				{
					if (pData->bArm)
					{
						m_pMakeGuess=GetGameGuiManager()->AddMakeGuess(stPointI(GetClientX()+100,GetClientY()+100));
						m_pMakeGuess->InitData();
						m_bProcess=true;
					}
				}
			}
			*/
		}
		break;

	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case ProcessFinish:
		case MakeButton:	
			{
				//if (m_pMake->IsEnabled())
				if ( m_bMoneyOk )
				{
					if (SendCmd())
					{
						SetEnabled(false);
					}
				}
				else
				{
                    ShowMoneyLackInfo();
				}
			}
			break;

		case ChangeNum:
			{
				Engine_WriteLogF("CGuiNpcMake::OnGuiEvent ChangeNum被点击:\n");
				switch(GetChangeNumBySingle()) {
				case 5:
					GetButton(ChangeNum)->GetToolTips()->Clear();
					GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数3\n合成成功率50%");
					GetButton(ChangeNum)->GetToolTips()->Resize();
					GetButton(ChangeNum)->SetText("3");
					
					break;
				case 4:
					GetButton(ChangeNum)->SetText("5");
					GetButton(ChangeNum)->GetToolTips()->Clear();
					GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数5\n合成成功率100%");
					GetButton(ChangeNum)->GetToolTips()->Resize();
					break;
				case 3:
					GetButton(ChangeNum)->SetText("4");
					GetButton(ChangeNum)->GetToolTips()->Clear();
					GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数4\n合成成功率75%");
					GetButton(ChangeNum)->GetToolTips()->Resize();
					break;
				}
				LoadData(m_params,true);
				
			}
			break;
		case EnableMakeButton:
			{
				m_bEnableMake = !m_bEnableMake;
				if (m_bEnableMake)
				{
					GetButton(EnableMakeButton)->SetText(SHOWALLMAKE);
				}
				else
				{
					GetButton(EnableMakeButton)->SetText(SHOWENABLEMAKE);
				}
				RefreshPropDataByGroup();
			}
			break;
		
		default:
			MaterialBtnClick(nID);
			UpdateInfo();
			break;
		}
		break;

	case EVENT_LISTBOX_SELECTION:
		{
			switch(nID) {
				case PropListBox:
				{
					stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
					if (!pData)
					{
						InitMaterial();
						break;
					}

					switch(m_eState) {
					case CGuiNpcDialog::eKaChange:
						{
							stPointI* pt=(stPointI*)malloc(sizeof(stPointI)*pData->nLevel*5+1);
							
							int j=0;
							for (int i=0;i<pData->nLevel;i++)
							{
								pt[j].x=KASTONE;
								pt[j].y=i;
								j++;
							}
							for (int i=0;i<pData->nLevel;i++)
							{
								pt[j].x=KASTONE1;
								pt[j].y=i;
								j++;
							}
							for (int i=0;i<pData->nLevel;i++)
							{
								pt[j].x=KASTONE2;
								pt[j].y=i;
								j++;
							}
							for (int i=0;i<pData->nLevel;i++)
							{
								pt[j].x=KASTONE3;
								pt[j].y=i;
								j++;
							}
							for (int i=0;i<pData->nLevel;i++)
							{
								pt[j].x=KASTONE4;
								pt[j].y=i;
								j++;
							}


							pt[j].x=RATESTONE;
							pt[j].y=pData->nLevel-1;

							RefreshMaterialData(pData->nLevel+1,pt);

							free(pt);
						}
						break;
					case CGuiNpcDialog::eChange:
						{
							char szText[MAX_NAMESIZE]={0};
							std::vector<std::string> strList;
							sprintf(szText,"id[%d]",pData->ObjectID);
							strList.push_back(szText);
							SetMakeParam(strList);


							stPointI pt[1]={stPointI(pData->ObjectID,pData->nLevel-1)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eUp:
						{
							//stPointI pt[1]={stPointI(UPOBJCET,pData->nLevel)};
							stPointI pt[1]={stPointI(UPOBJCET,0)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eBackout:
						{
							char szText[MAX_NAMESIZE]={0};
							std::vector<stPointI>  stList;
						
							m_MakeData.clearMaterial();


							t_Object* pItemObject = ((CRoleItem*)pData->pObject)->GetObject();
							
							if( (pItemObject->maker)[0] )
							{
								ObjectBase_t* pObject = pData->GetObjectBase();
								std::vector<UINT, std::stack_allocator<UINT> > aid;
								aid.reserve(16);

								_parse_str_num(aid,pObject->strField36);

								std::string str;
								if (aid.size()>0)
								{
									//m_MakeData.insertMaterial(MONEYOBJECT,aid[0],aid[0],1);
									str+="id[";
									int i;
									for (i=1;i<aid.size();i+=3)
									{
										if (i!=1)
											str+=",";
										str+=_itoa(aid[i],szText,10);
										m_MakeData.insertMaterial(aid[i],aid[i+2],aid[i+2],1);
										stList.push_back(stPointI(aid[i],aid[i+1]));
									}
									str+="]";
								}

								std::vector<std::string> strList;
								strList.push_back(str);
								SetMakeParam(strList);
								
								stPointI* pt=(stPointI*)malloc(sizeof(stPointI)*stList.size());
								for (int i=0;i<stList.size();i++)
								{
									pt[i]=stList[i];
								}
								RefreshMaterialData(stList.size(),pt);
								free(pt);
							}
							else
							{
								InitMaterial();
								break;
							}

						}
						break;
					
					case CGuiNpcDialog::eMakeFood:
					case CGuiNpcDialog::eMakeDrug:
					case CGuiNpcDialog::eMake:
						{
							char szText[MAX_NAMESIZE]={0};
							std::vector<stPointI>  stList;
							m_MakeData.clearMaterial();

							ObjectBase_t* pObject = pData->GetObjectBase();
							DWORD ObjectID=m_MakeData.getMakeRateObject(pObject->dwType);
							std::vector<UINT, std::stack_allocator<UINT> > aid;
							aid.reserve(16);

							_parse_str_num(aid,pObject->strField36);
							
							std::string str;
							if (aid.size()>0)
							{
								//m_MakeData.insertMaterial(MONEYOBJECT,aid[0],aid[0],1);
								str+="id[";
								int i;
								for (i=1;i<aid.size();i+=3)
								{
									if (i!=1)
										str+=",";
									str+=_itoa(aid[i],szText,10);
									m_MakeData.insertMaterial(aid[i],aid[i+2],aid[i+2],2);
									stList.push_back(stPointI(aid[i],aid[i+1]));
								}
								if (ObjectID!=0)
								{
									if (i!=1)
										str+=",";
									str+=_itoa(ObjectID,szText,10);
									m_MakeData.insertMaterial(ObjectID,1,1,0);
									stList.push_back(stPointI(ObjectID,0));
								}
								str+="]";
							}

							std::vector<std::string> strList;
							strList.push_back(str);
							SetMakeParam(strList);

							stPointI* pt=(stPointI*)malloc(sizeof(stPointI)*stList.size());
							for (int i=0;i<stList.size();i++)
							{
								pt[i]=stList[i];
							}
							RefreshMaterialData(stList.size(),pt);
							free(pt);
			
						}
						break;
					case CGuiNpcDialog::eKaMake:
						{	
							stPointI pt[1]={stPointI(MAKESTONE,pData->nUseSocket)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eHoleMake:
						{
							stPointI pt[1]={stPointI(HOLESTONE,pData->nCanSocket+pData->nUseSocket)};
							RefreshMaterialData(1,pt);
						}
						break;
					//soke 改造装备
					case CGuiNpcDialog::eModifyAttri:           // 改造属性
					case CGuiNpcDialog::eAddReBind:			    // 重新绑定
					case CGuiNpcDialog::eAddReMaker:			// 修改签名
					case CGuiNpcDialog::eAddAddMaker:			// 装备签名
					case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
					case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造	
					case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
					case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
					case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
					case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
						{
							CRoleItem* pItem = (CRoleItem*)pData->pObject;
							DWORD dwLevel = 0;
							if(pItem->GetObject()->kind & 16)
								dwLevel = 5;//改造材料等级
							else if(pItem->GetObject()->kind & 4)
								dwLevel = 5;//改造材料等级

							char szText[64]={0};
							std::vector<std::string> strList;
							sprintf(szText,"id[%d]",MODIFYSTONE);
							strList.push_back(szText);
							SetMakeParam(strList);

							stPointI pt[1]={stPointI(MODIFYSTONE,dwLevel)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eAddEquipMake:			// 装备祝福改造
						{
							CRoleItem* pItem = (CRoleItem*)pData->pObject;
							DWORD dwLevel = 0;
							if(pItem->GetObject()->kind & 4)
								dwLevel = 0;

							char szText[64]={0};
							std::vector<std::string> strList;
							sprintf(szText,"id[%d]",MAKEZSTONE);
							strList.push_back(szText);
							SetMakeParam(strList);

							stPointI pt[1]={stPointI(MAKEZSTONE,dwLevel)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eAddEquipZsMake:			// 马匹装备祝福改造
						{
							CRoleItem* pItem = (CRoleItem*)pData->pObject;
							DWORD dwLevel = 0;
							if(pItem->GetObject()->kind & 4)
								dwLevel = 0;

							char szText[64]={0};
							std::vector<std::string> strList;
							sprintf(szText,"id[%d]",MAKEZQTONE);
							strList.push_back(szText);
							SetMakeParam(strList);

							stPointI pt[1]={stPointI(MAKEZQTONE,dwLevel)};
							RefreshMaterialData(1,pt);
						}
						break;
					case CGuiNpcDialog::eAddEquipMaklv:			// 装备升级改造
						{
							char szText[MAX_NAMESIZE]={0};
							std::vector<stPointI>  stList;
							m_MakeData.clearMaterial();

							ObjectBase_t* pObject = pData->GetObjectBase();
							DWORD ObjectID=m_MakeData.getMakeRateObject(pObject->dwType);
							std::vector<UINT, std::stack_allocator<UINT> > aid;
							aid.reserve(16);
							_parse_str_num(aid,pObject->strField36);

							CRoleItem* pItem = (CRoleItem*)pData->pObject;
							DWORD dwLevel = 0;
							if(pItem->GetObject()->kind & 16)
								dwLevel = 5;//改造材料等级
							else if(pItem->GetObject()->kind & 4)
								dwLevel = 5;//改造材料等级

							std::string str;
							str+="id[";
							str+=_itoa(MAKELVTONE,szText,10);
							m_MakeData.insertMaterial(MAKELVTONE,MAKELVNUM,MAKELVNUM,1);
							stList.push_back(stPointI(MAKELVTONE,dwLevel));

							WORD needLevel = pItem->GetObject()->needlevel;
							if(needLevel > 160 && aid.size()>0)
							{
								int index = aid.size() - 3;
								str+=",";
								str+=_itoa(aid[index],szText,10);
								m_MakeData.insertMaterial(aid[index],aid[index+2],aid[index+2],1);
								stList.push_back(stPointI(aid[index],aid[index+1]));
							}
							str+="]";

							std::vector<std::string> strList;
							strList.push_back(str);
							SetMakeParam(strList);

							stPointI* pt=(stPointI*)malloc(sizeof(stPointI)*stList.size());
							for (int i=0;i<stList.size();i++)
							{
								pt[i]=stList[i];
							}
							RefreshMaterialData(stList.size(),pt);
							free(pt);
							
						}
						break;
					}
				}
				break;
			}
			RefreshGui();
			UpdateInfo();
		}
		break;

	case EVENT_LISTBOX_CHANGED:
		{
			switch(nID) {
			case MaterialListBox:
				RefreshGui();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 检查是否可灵魂合成
 * 
 * 检查是否可灵魂合成
 * 
 * \param condition : 被检查数值
 * \param n : 数组大小
 * \param pPt : 数组
 * \return 检查是否可灵魂合成的结果
 */
bool CGuiNpcMake::CheckConditionByKaChange(stPointI condition,int n,stPointI* pPt)
{
	FUNCTION_BEGIN;

	if ((m_eState!=CGuiNpcDialog::eKaChange)/*&&(m_eState!=CGuiNpcDialog::eMake)&&(m_eState!=CGuiNpcDialog::eMakeFood)*/)
		return true;

	bool bRet = false;

	for (int i=0;i<n;i++)
	{
		if (condition.x==pPt[i].x)
		{
			if (condition.y==pPt[i].y)
			{
				bRet = true;
				break;
			}
		}
	}
	return bRet;

/**
 * \brief 触发材料加减按钮事件
 * 
 * 触发材料加减按钮事件
 * 
 * \param nID : 按钮ID
 * \return 无
 */
	FUNCTION_END;
}
void CGuiNpcMake::MaterialBtnClick(UINT nID)
{
	FUNCTION_BEGIN;

	int index=-1;
	bool bAdd=false;
	if ((nID>=ADDBUTTON)&&(nID<LESSBUTTON))
	{
		index = nID - ADDBUTTON;
		bAdd = true;
	}
	else if (nID>=LESSBUTTON)
	{
		index = nID - LESSBUTTON;
	}

	if ((index<0)||(index>=m_aBtnGroupList.size()))
		return;

	int i=m_aBtnGroupList[index].index;

	if (bAdd)
	{
		if (m_aMaterialInfoList[i].nUseNum==0)
		{
			m_MakeData.insertSelectData(&m_aMaterialInfoList[i]);
		}
		m_aMaterialInfoList[i].nLeaveNum-=m_aMaterialInfoList[i].nStepNum;
		m_aMaterialInfoList[i].nUseNum+=m_aMaterialInfoList[i].nStepNum;
	}
	else
	{
		m_aMaterialInfoList[i].nLeaveNum+=m_aMaterialInfoList[i].nStepNum;
		m_aMaterialInfoList[i].nUseNum-=m_aMaterialInfoList[i].nStepNum;
		if (m_aMaterialInfoList[i].nUseNum==0)
		{
			m_MakeData.deleteSelectData(&m_aMaterialInfoList[i]);
		}
	}
	
	
	ResetListItem(i);
	//ResetButtonState(index);
	RefreshGui();
	


	FUNCTION_END;
}

/**
* \brief 创建材料加减按钮
* 
* 创建材料加减按钮
* 
* \return 无
*/
void CGuiNpcMake::CreateButtons()
{
	FUNCTION_BEGIN;

	if (m_pMaterial==NULL)
		return;

	m_aBtnGroupList.clear();
	int xPos = m_pMaterial->GetX() + m_pMaterial->GetWidth() - 60 ;
	int yPos = m_pMaterial->GetY() ;
	int yHeight = m_pMaterial->GetY() + m_pMaterial->GetHeight() - 15 ;

	int xOffset = 17;
	int yOffset = 16;


	CGuiButton* pButton = NULL;
	stResourceLocation rl;
	rl.SetFileName( "data\\interfaces.gl");
	rl.group = 0;
	
	
	int i = 0;
	do {
		stBtnGroup  st;
		st.index = -1;
		rl.frame = 21;
		AddButton(ADDBUTTON+i,"",xPos,yPos,0,0,NULL,false,&pButton);
		pButton->SetButtonType(CGuiButton::ButtonType_Bitmap);
		pButton->SetBitmap(rl,1);

		st.pButtonAdd = pButton;
		rl.frame = 24;
		//pButton-
		AddButton(LESSBUTTON+i,"",xPos+xOffset,yPos,0,0,NULL,false,&pButton);
		pButton->SetButtonType(CGuiButton::ButtonType_Bitmap);
		pButton->SetBitmap(rl,1);
		st.pButtonLess = pButton;

		m_aBtnGroupList.push_back(st);

		yPos+=yOffset;
		i++;
	} while(yPos < yHeight );
	

	FUNCTION_END;
}

/**
 * \brief 设置要打造数据
 * 
 * 设置要打造数据
 * 
 * \param params : 服务器发送的打造数据
 * \param n : 数组大小
 * \param pPt : 数组 x代表物品ID y代表物品等级
 * \return 无
 */
void CGuiNpcMake::SetMakeParam(std::vector<std::string> & params,int n,stPointI* pPt)
{
	FUNCTION_BEGIN;

	m_aMakeObject.clear();
	if( params.size() > 0 )
	{
		ParseMakeString(m_aMakeObject,params[0].c_str(),m_eState);
	}
	
	RefreshMaterialData(n,pPt);
	RefreshGui();

	GetGameState()->SetStateItem(eGameStateItem_Up);


	FUNCTION_END;
}

/**
 * \brief 初始化打造数据,材料数据
 * 
 * 初始化打造数据,材料数据
 * 
 * \param params : 服务器发送的打造数据
 * \return 无
 */
void CGuiNpcMake::SetMakeToParam(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	char szText[64]={0};
	m_aMakeToObject.clear();
	if( params.size() > 0 )
	{
		ParseMakeString(m_aMakeToObject,params[0].c_str(),m_eState);
	}

	
	
	switch(m_eState) {
	case CGuiNpcDialog::eChange:
		{
			RefreshPropData(true);
		}
		break;
	case CGuiNpcDialog::eKaChange:
		{

			//SetMakeParam(params);
			std::vector<std::string> strList;
			sprintf(szText,"id[%d,%d]",KASTONE,RATESTONE);
			strList.push_back(szText);
			SetMakeParam(strList);

			RefreshPropData();
		}
		break;
	case CGuiNpcDialog::eUp:
		{
			std::vector<std::string> strList;
			sprintf(szText,"id[%d,%d]",UPOBJCET,RATEOBJECT);
			strList.push_back(szText);
			SetMakeParam(strList);

			RefreshPropData();
		}
		break;
	case CGuiNpcDialog::eMakeFood:
	case CGuiNpcDialog::eMakeDrug:
	case CGuiNpcDialog::eMake:
		{
			RefreshPropData(true);
		}
		break;
	case CGuiNpcDialog::eKaMake:
		{
			std::vector<std::string> strList;
			sprintf(szText,"id[%d,%d]",MAKESTONE,KASTONE);
			strList.push_back(szText);
			SetMakeParam(strList);

			RefreshPropData();
		}
		break;
	case CGuiNpcDialog::eHoleMake:
		{
			std::vector<std::string> strList;
			sprintf(szText,"id[%d]",HOLESTONE);
			strList.push_back(szText);
			SetMakeParam(strList);

			RefreshPropData(true);
		}
		break;
	case CGuiNpcDialog::eBackout:
		{
			RefreshPropData(true);
		}
		break;
	//soke  改造
	case CGuiNpcDialog::eModifyAttri:           // 改造属性
	case CGuiNpcDialog::eAddReBind:			    // 重新绑定
	case CGuiNpcDialog::eAddReMaker:			// 修改签名
	case CGuiNpcDialog::eAddAddMaker:			// 装备签名
	case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
	case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造
	case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
	case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
	case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
	case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
	{
		std::vector<std::string> strList;
		sprintf(szText,"id[%d]",MODIFYSTONE);
		strList.push_back(szText);
		SetMakeParam(strList);
		RefreshPropData(true);
	}
	break;
	case CGuiNpcDialog::eAddEquipMake:			// 装备祝福改造
	{
		std::vector<std::string> strList;
		sprintf(szText,"id[%d]",MAKEZSTONE);
		strList.push_back(szText);
		SetMakeParam(strList);
		RefreshPropData(true);
	}
	break;
	case CGuiNpcDialog::eAddEquipZsMake:			// 马匹装备祝福改造
	{
		std::vector<std::string> strList;
		sprintf(szText,"id[%d]",MAKEZQTONE);
		strList.push_back(szText);
		SetMakeParam(strList);
		RefreshPropData(true);
	}
	break;
	case CGuiNpcDialog::eAddEquipMaklv:			// 装备升级改造
	{
		RefreshPropData(true);
	}
	break;

	}
	
	RefreshGui();

	GetGameState()->SetStateItem(eGameStateItem_Up);

	FUNCTION_END;
}

/**
 * \brief 刷新材料加减按钮状态
 * 
 * 刷新材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::RefreshGui()
{
	FUNCTION_BEGIN;

	
	for (int i=0;i<m_aBtnGroupList.size();i++)
	{
		m_aBtnGroupList[i].index=-1;
		m_aBtnGroupList[i].pButtonAdd->SetVisible(false);
		m_aBtnGroupList[i].pButtonLess->SetVisible(false);
		
		int index = i + m_pMaterial->GetFirstShowItem();
		if (m_pMaterial->GetCount()<=0)
			continue;

		if (index > m_pMaterial->GetLastShowItem())
			continue;

		m_aBtnGroupList[i].index = index;
		m_aBtnGroupList[i].pButtonAdd->SetVisible(true);
		m_aBtnGroupList[i].pButtonLess->SetVisible(true);
		ResetButtonState(i);
	}
	

	UpdateGui();

	FUNCTION_END;
}

/**
 * \brief 刷新要打造的数据内容
 * 
 * 刷新要打造的数据内容
 * 
 * \param bGroup : 是否分类显示
 * \return 无
 */
void CGuiNpcMake::RefreshPropData(bool bGroup)
{
	FUNCTION_BEGIN;

	m_aPropInfoList.clear();


	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();

	
	for(size_t i=0; i < m_aMakeToObject.size(); i++)
	{
		switch(m_eState) {
		case CGuiNpcDialog::eUp:
			{
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON 
						&& (*it)->GetObject()->upgrade < m_aMakeToObject[i]->dwUp)
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
			}
			break;
		case CGuiNpcDialog::eKaChange:
			{
				if (m_aMakeToObject[i]->dwID==KASTONE)
				{
					for (int j=1;j<=m_aMakeToObject[i]->dwChange;j++)
					{
						stPropInfo st;
						st.ObjectID = m_aMakeToObject[i]->dwID;
						st.bObject = true;
						st.nLevel = j;
						st.pObject = m_aMakeToObject[i];
						st.type = m_aMakeToObject[i]->maketype;
						m_MakeData.GetNameAndLevel(st.name,m_aMakeToObject[i]->strName,st.nLevel,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
			}
			break;
		case CGuiNpcDialog::eChange:
			{
				for (int j=1;j<=m_aMakeToObject[i]->dwChange;j++)
				{
					stPropInfo st;
					st.ObjectID = m_aMakeToObject[i]->dwID;
					st.bObject = true;
					st.nLevel = j;
					st.pObject = m_aMakeToObject[i];
					st.type = m_aMakeToObject[i]->maketype;
					m_MakeData.GetNameAndLevel(st.name,m_aMakeToObject[i]->strName,st.nLevel,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
					m_aPropInfoList.push_back(st);
				}
			}
			break;

		case CGuiNpcDialog::eMake:
		case CGuiNpcDialog::eMakeFood:
		case CGuiNpcDialog::eMakeDrug:
			{
				stPropInfo st;
				
				st.ObjectID = m_aMakeToObject[i]->dwID;
				st.bObject = true;
				st.nLevel = 0;
				st.pObject = m_aMakeToObject[i];
				st.type = m_aMakeToObject[i]->maketype;
				if ((m_aMakeToObject[i]->dwType!=ItemType_Resource)&&(m_aMakeToObject[i]->dwType!=ItemType_Food))
					st.bArm=true;
				m_MakeData.GetNameAndLevel(st.name,m_aMakeToObject[i]->strName,0,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
				m_aPropInfoList.push_back(st);
			}
			break;
		case CGuiNpcDialog::eKaMake:
			{
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
					{
						stPropInfo st;
						st.nCanSocket=m_MakeData.CheckKaMake((*it)->GetObject()->socket,st.nUseSocket);
						if (st.nCanSocket!=0)
						{
							st.ObjectID = (*it)->GetObject()->qwThisID;
							st.nLevel = (*it)->GetObject()->upgrade;
							st.type = (*it)->GetObjectBase()->maketype;
							st.pObject = (*it);
							memcpy(st.socket,(*it)->GetObject()->socket,sizeof(st.socket));
							m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
							m_aPropInfoList.push_back(st);
						}
					}
				}
			}
			break;
		case CGuiNpcDialog::eHoleMake:
			{
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
					{
						stPropInfo st;
						st.nCanSocket=m_MakeData.CheckKaMake((*it)->GetObject()->socket,st.nUseSocket);
						if (st.nCanSocket+st.nUseSocket<SOCKETNUM)
						{
							st.ObjectID = (*it)->GetObject()->qwThisID;
							st.nLevel = (*it)->GetObject()->upgrade;
							st.type = (*it)->GetObjectBase()->maketype;
							st.pObject = (*it);
							memcpy(st.socket,(*it)->GetObject()->socket,sizeof(st.socket));
							m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
							m_aPropInfoList.push_back(st);
						}
					}
				}
			}
			break;
		case CGuiNpcDialog::eBackout:
			{
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
			}
			break;
		//soke 装备改造
		case CGuiNpcDialog::eModifyAttri:           // 改造属性
		case CGuiNpcDialog::eAddReBind:			    // 重新绑定
		case CGuiNpcDialog::eAddReMaker:			// 修改签名
		case CGuiNpcDialog::eAddAddMaker:			// 装备签名
		case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
		case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造
		case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
		case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
		case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
		case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
		{
			    int nSize = pMainRole->m_listItem.size();
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					DWORD id = (*it)->GetObjectID();
					const char *name = (*it)->GetName();
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON && (*it)->IsBind())
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
		}
		break;
		case CGuiNpcDialog::eAddEquipMake:			// 装备祝福
		{
			    int nSize = pMainRole->m_listItem.size();
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					DWORD id = (*it)->GetObjectID();
					const char *name = (*it)->GetName();
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON && (*it)->IsBind())
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
		}
		break;
	    case CGuiNpcDialog::eAddEquipZsMake:			// 马匹装备祝福
		{
			    int nSize = pMainRole->m_listItem.size();
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					DWORD id = (*it)->GetObjectID();
					const char *name = (*it)->GetName();
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON && (*it)->IsBind())
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
		}
		break;
		case CGuiNpcDialog::eAddEquipMaklv:			// 装备升级
		{
			    int nSize = pMainRole->m_listItem.size();
				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					DWORD id = (*it)->GetObjectID();
					const char *name = (*it)->GetName();
					if((*it)->GetObjectID() == m_aMakeToObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON && (*it)->IsBind())
					{
						stPropInfo st;
						st.ObjectID = (*it)->GetObject()->qwThisID;
						st.nLevel = (*it)->GetObject()->upgrade;
						st.type = (*it)->GetObjectBase()->maketype;
						st.pObject = (*it);
						m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeToObject[i]->dwChange+m_aMakeToObject[i]->dwUp);
						m_aPropInfoList.push_back(st);
					}
				}
		}
		break;

		}
	}
	
	

	if (!bGroup)
	{
		RefreshPropDataByNormal();
	}
	else
	{
		RefreshPropDataByGroup();
	}

	FUNCTION_END;
}

/**
 * \brief 设置默认的打造物品
 * 
 * 设置默认的打造物品
 * 
 * \return 无
 */
void CGuiNpcMake::SelectDefProp()
{
	FUNCTION_BEGIN;

	if (m_pProp->GetItemCount()>0)
	{
		for (int i=0;i<m_pProp->GetItemCount();i++)
		{
			if (m_pProp->GetItemData(i)!=NULL)
			{
				m_pProp->SelectItem(i);
				//OnGuiEvent(EVENT_LISTBOX_SELECTION,m_pProp->GetID(),m_pProp);
				break;
			}
		}
	}
	else
		InitMaterial();

	FUNCTION_END;
}


/**
 * \brief 初始化材料数据
 * 
 * 初始化材料数据
 * 
 * \return 无
 */
void CGuiNpcMake::InitMaterial()
{
	FUNCTION_BEGIN;

	std::vector<std::string> strList;
	strList.push_back("id[]");
	SetMakeParam(strList);

	FUNCTION_END;
}

/**
 * \brief 无分类刷新打造物品
 * 
 * 无分类刷新打造物品
 * 
 * \return 无
 */
void CGuiNpcMake::RefreshPropDataByNormal()
{
	FUNCTION_BEGIN;

	m_pProp->RemoveAllItems();
	for (int i=0;i<m_aPropInfoList.size();i++)
	{
		bool bMake = false;

		switch(m_eState) {
		case CGuiNpcDialog::eUp:
			{
				bMake=m_MakeData.IsUpItem(&(m_aPropInfoList[i]));
				
				DWORD color = bMake?GREENCOLOR:REDCOLOR;
				m_pProp->AddItem(m_aPropInfoList[i].name,(void*)&(m_aPropInfoList[i]));
				m_pProp->SetItemColor(m_pProp->GetItemCount()-1,0,color);
			}
			break;
		case CGuiNpcDialog::eKaMake:
			{
				bMake=m_MakeData.IsKaMake(&(m_aPropInfoList[i]));
				
				DWORD color = bMake?GREENCOLOR:REDCOLOR;
				m_pProp->AddItem(m_aPropInfoList[i].name,(void*)&(m_aPropInfoList[i]));
				m_pProp->SetItemColor(m_pProp->GetItemCount()-1,0,color);
			}
			break;
		case CGuiNpcDialog::eKaChange:
			{
				bMake=m_MakeData.IsKaChange(&(m_aPropInfoList[i]));

				if (bMake)
				{
					m_pProp->AddItem(m_aPropInfoList[i].name,(void*)&(m_aPropInfoList[i]));
					m_pProp->SetItemColor(m_pProp->GetItemCount()-1,0,GREENCOLOR);
				}
			}
			break; 
		}

	}
	
	SelectDefProp();


	FUNCTION_END;
}

/**
* \brief 分类刷新打造物品
* 
* 分类刷新打造物品
* 
* \return 无
*/
void CGuiNpcMake::RefreshPropDataByGroup()
{
	FUNCTION_BEGIN;

	static int countGroup = sizeof(g_GroupInfo)/sizeof(stGroupInfo);
	static int countItemType = sizeof(g_ItemtypeInGroup)/sizeof(stItemTypeInGroup);

	m_pProp->RemoveAllItems();
	
	for (int k=0;k<m_aPropInfoList.size();k++)
	{
		m_aPropInfoList[k].bAddIn = false;
	}

	for (int i=0;i<countGroup;i++)
	{
		bool bHave = false;
		int  nPos = m_pProp->GetItemCount();

		for (int j=0;j<countItemType;j++)
		{
			if (g_GroupInfo[i].id==g_ItemtypeInGroup[j].groupid)
			{
				for (int k=0;k<m_aPropInfoList.size();k++)
				{
					if (m_aPropInfoList[k].bAddIn)
						continue;

					if (g_ItemtypeInGroup[j].itemtype == m_aPropInfoList[k].type)
					{
						char* pStr = NULL;

						switch(m_eState) {
						case CGuiNpcDialog::eMakeFood:
						case CGuiNpcDialog::eMakeDrug:
						case CGuiNpcDialog::eMake:
							{
								bool  bMake = m_MakeData.IsMakeItem(m_aPropInfoList[k].GetObjectBase()->strField36,
									m_aPropInfoList[k].GetObjectBase()->needLevel);
								bool  bCanSkill = true;//m_MakeData.IsCanSkill(m_aPropInfoList[k].GetObjectBase()->strField35,&pStr);
								//if ((!m_bEnableMake)||(bMake&&bCanSkill&&pStr))
								if ((!m_bEnableMake)||(bMake&&bCanSkill))
								{
									DWORD color=bMake?GREENCOLOR:REDCOLOR;
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);
									
									/*
									if (pStr)
									{
										color=bCanSkill?GREENCOLOR:REDCOLOR;
										m_pProp->SetItemText(m_pProp->GetItemCount()-1,2,pStr);
										m_pProp->SetItemColor(m_pProp->GetItemCount()-1,2,color);
									}
									*/
									
									bHave = true;
								}
							}
							break;

						case CGuiNpcDialog::eBackout:
							{
								bool  bBackout = m_MakeData.CheckMoney(m_MakeData.GetBackoutMoney((CRoleItem*)m_aPropInfoList[k].pObject));
								
								//if (bBackout)
								{
									DWORD color=bBackout?GREENCOLOR:REDCOLOR;
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);

									bHave = true;
								}
							}
							break;
						
						case CGuiNpcDialog::eHoleMake:
							{
								bool bMake=m_MakeData.IsHoleMake(&(m_aPropInfoList[k]));
								DWORD color=bMake?GREENCOLOR:REDCOLOR;
								//if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name,20);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);

									char szTxt[MAX_NAMESIZE];
									sprintf(szTxt,"%d 凹槽",m_aPropInfoList[k].nUseSocket + m_aPropInfoList[k].nCanSocket );
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,2,szTxt);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,2,color);

									bHave = true;
								}
								
							}
							break;

						case CGuiNpcDialog::eChange:
							{
								bool bMake=m_MakeData.IsChangeItem(&(m_aPropInfoList[k]),GetChangeNumBySingle());
								if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,GREENCOLOR);

									bHave = true;
								}
							}
							break;
						//soke 改造装备
						case CGuiNpcDialog::eModifyAttri:
						case CGuiNpcDialog::eAddReBind:
						case CGuiNpcDialog::eAddReMaker:
						case CGuiNpcDialog::eAddAddMaker:
						case CGuiNpcDialog::eAddModifySoulA:
						case CGuiNpcDialog::eAddLVSoullineMD:
						case CGuiNpcDialog::eAddModifyDxSoulA:
						case CGuiNpcDialog::eAddMDSkillA:
						case CGuiNpcDialog::eAddEquipRound:
						case CGuiNpcDialog::eAddMDMake:
							{
								CRoleItem *pItem = (CRoleItem*)m_aPropInfoList[k].pObject;
								DWORD dwLevel = 0;
								bool bMake = false;
								if(pItem->GetObject()->kind & 16)
									dwLevel = 5;  //改造材料等级
								else if(pItem->GetObject()->kind & 4)
									dwLevel = 5; //改造材料等级
								if(dwLevel != 0)
									bMake=m_MakeData.IsCanModify(m_aPropInfoList[k].GetObjectBase()->strField36,dwLevel);
								DWORD color=bMake?GREENCOLOR:REDCOLOR;
								//if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);
									bHave = true;
								}

							}
							break;
						case CGuiNpcDialog::eAddEquipMake: //soke 装备祝福
							{
								CRoleItem *pItem = (CRoleItem*)m_aPropInfoList[k].pObject;
								DWORD dwLevel = 0;
								bool bMake = false;
								if(pItem->GetObject()->kind & 4)
									dwLevel = 0;
								if(dwLevel != 0)
									bMake=m_MakeData.IsCanMakezs(m_aPropInfoList[k].GetObjectBase()->strField36,dwLevel);
								DWORD color=bMake?REDCOLOR:GREENCOLOR; //soke 装备祝福颜色
								//if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);
									bHave = true;
								}

							}
							break;
						case CGuiNpcDialog::eAddEquipZsMake: //soke 马匹装备祝福
							{
								CRoleItem *pItem = (CRoleItem*)m_aPropInfoList[k].pObject;
								DWORD dwLevel = 0;
								bool bMake = false;
								if(pItem->GetObject()->kind & 4)
									dwLevel = 0;
								if(dwLevel != 0)
									bMake=m_MakeData.IsCanMakezq(m_aPropInfoList[k].GetObjectBase()->strField36,dwLevel);
								DWORD color=bMake?REDCOLOR:GREENCOLOR; //soke 装备祝福颜色
								//if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);
									bHave = true;
								}

							}
							break;
						case CGuiNpcDialog::eAddEquipMaklv: //soke 装备升级
						{
							CRoleItem *pItem = (CRoleItem*)m_aPropInfoList[k].pObject;
								DWORD dwLevel = 0;
								bool bMake = false;
								if(pItem->GetObject()->kind & 16)
									dwLevel = 5;//改造材料等级
								else if(pItem->GetObject()->kind & 4)
									dwLevel = 5;//改造材料等级
								if(dwLevel != 0)
									bMake=m_MakeData.IsCanMakelv(m_aPropInfoList[k].GetObjectBase()->strField36,dwLevel,m_aPropInfoList[k].GetObjectBase()->needLevel);
								DWORD color=bMake?GREENCOLOR:REDCOLOR;
								//if (bMake)
								{
									m_pProp->AddItem("",(void*)&(m_aPropInfoList[k]));
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,0,"",4);
									m_pProp->SetItemText(m_pProp->GetItemCount()-1,1,m_aPropInfoList[k].name);
									m_pProp->SetItemColor(m_pProp->GetItemCount()-1,1,color);
									bHave = true;
								}

						}
							break;
						}
								
						m_aPropInfoList[k].bAddIn=true;
					}
				}
			}
		}

		if (bHave)
		{
			m_pProp->InsertItem(nPos,g_GroupInfo[i].name,NULL);
			m_pProp->SetItemColor(nPos,0,YELLOWCOLOR);
		}
	}

	SelectDefProp();

	FUNCTION_END;
}


/**
 * \brief 刷新材料数据
 * 
 * 刷新材料数据
 * 
 * \param n : 数组大小
 * \param pPt : 数组
 * \return 无
 */
void CGuiNpcMake::RefreshMaterialData(int n,stPointI* pPt)
{
	FUNCTION_BEGIN;

	m_aMaterialInfoList.clear();
	m_MakeData.clearSelectInfo();
	
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	

	for(size_t i=0; i < m_aMakeObject.size(); i++)
	{
		/*
		
		*/
		bool  bFind = false;
		std::vector<UINT, std::stack_allocator<UINT> >  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,eMaterial),
			m_aMakeObject[i]->dwID,"1,1,0");
		if (ainfo.size()<=0)
			continue;
		
		if (m_eState==CGuiNpcDialog::eChange)
		{
			ainfo[0]=GetChangeNumBySingle();
			CRoleItem*  pRoleItem = GetScene()->GetMainCharacter()->FindItemByID(m_dwChangeID);
			if (pRoleItem)
			{
				int nMaxCount=pRoleItem->GetObject()->dur*ainfo[0];
				if (nMaxCount==0)
					Close();

				ainfo[1]=nMaxCount<ainfo[1]?nMaxCount:ainfo[1];
			}
			else
				Close();
			
		}
		//soke 改造需求
		else if(m_eState == CGuiNpcDialog::eModifyAttri||
				m_eState == CGuiNpcDialog::eAddReBind||
				m_eState == CGuiNpcDialog::eAddReMaker||
				m_eState == CGuiNpcDialog::eAddAddMaker||
				m_eState == CGuiNpcDialog::eAddModifySoulA||
				m_eState == CGuiNpcDialog::eAddLVSoullineMD||
				m_eState == CGuiNpcDialog::eAddModifyDxSoulA||
				m_eState == CGuiNpcDialog::eAddMDSkillA||
				m_eState == CGuiNpcDialog::eAddEquipRound||
				m_eState == CGuiNpcDialog::eAddMDMake)
		{
			ainfo[0] = 2;//改造需要的数量
			ainfo[1] = 2;//最大数量
			ainfo[2] = 1;
		}
		//soke 祝福改造需求
		else if( m_eState == CGuiNpcDialog::eAddEquipMake)
		{
			ainfo[0] = 5;//祝福改造需要的数量
			ainfo[1] = 5;//最大数量
			ainfo[2] = 1;
		}
		//soke 马匹装备祝福改造需求
		else if( m_eState == CGuiNpcDialog::eAddEquipZsMake)
		{
			ainfo[0] = 5;//祝福改造需要的数量
			ainfo[1] = 5;//最大数量
			ainfo[2] = 1;
		}

		switch(ainfo[2]) 
		{
		case 0:
			{

				for(CMainCharacter::tListItem::iterator it = pMainRole->m_listItem.begin(); it != pMainRole->m_listItem.end(); ++ it)
				{
					if((*it)->GetObjectID() == m_aMakeObject[i]->dwID && (*it)->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
					{
						//if ((m_aMakeObject[i]->dwChange>0)&&(m_aMakeObject[i]->dwChange==(*it)->GetObject()->f))
						//	continue;
						//if (m_MakeData.CheckChange((*it)->GetObject()->upgrade,m_aMakeObject[i]->dwChange,m_eState))
						{
							if (CheckConditionByKaChange(stPointI(m_aMakeObject[i]->dwID,(*it)->GetObject()->upgrade),n,pPt))
							{
								stMaterialInfo st;
								st.pObject = (*it);
								st.ObjectID = (*it)->GetObject()->qwThisID;
								st.type = (*it)->GetObjectBase()->maketype;
								m_MakeData.GetNameAndLevel(st.name,(*it)->GetName(),(*it)->GetObject()->upgrade,st.type,m_aMakeObject[i]->dwChange+m_aMakeObject[i]->dwUp);
								st.nLeaveNum = (*it)->GetCount();
								st.nUseNum = 0;
								st.nLevel = (*it)->GetObject()->upgrade;
								st.nStepNum = ainfo[0];
								st.nNeedNum = st.nStepNum;
								st.nMaxUseNum = ainfo[1];
								st.bAdd=ainfo[2];
								bFind = true;

								m_aMaterialInfoList.push_back(st);
							}
						}
					}
				}
			}
			break;
		case 1:
			{
				if (m_eState==CGuiNpcDialog::eBackout)
				{
					char szTmp[128]={0};
					sprintf(szTmp,"    %s",m_aMakeObject[i]->strName);
					stMaterialInfo st;
					st.pObject = GetObjectBase(m_aMakeObject[i]->dwID);
					st.ObjectID = m_aMakeObject[i]->dwID;
					st.type = m_aMakeObject[i]->maketype;
					m_MakeData.GetNameAndLevel(st.name,szTmp,0,st.type,0);
					st.nLeaveNum = -1;
					st.nUseNum = -1;
					st.nLevel = -1;
					st.nStepNum = -1;
					st.nNeedNum = -1;
					st.nMaxUseNum = -1;
					st.bAdd = true;
					bFind = true;

					m_aMaterialInfoList.push_back(st);
					continue;
				}


				for (int j=0;j<=m_aMakeObject[i]->dwChange;j++)
				{
					//if (m_MakeData.CheckChange(j,m_aMakeObject[i]->dwChange,m_eState))
					{
						if (m_MakeData.CheckCondition(stPointI(m_aMakeObject[i]->dwID,j),n,pPt))
						{
							int nItemCount = pMainRole->GetItemCount(m_aMakeObject[i]->dwID,OBJECTCELLTYPE_COMMON,j);
							if (nItemCount > 0)
							{
								stMaterialInfo st;
								st.pObject = m_aMakeObject[i];
								st.ObjectID = m_aMakeObject[i]->dwID;
								st.type = m_aMakeObject[i]->maketype;
								m_MakeData.GetNameAndLevel(st.name,m_aMakeObject[i]->strName,j,st.type,m_aMakeObject[i]->dwChange+m_aMakeObject[i]->dwUp);
								st.nLeaveNum = nItemCount;
								st.nLevel = j;
								st.nUseNum = 0;
								st.nStepNum = ainfo[0];
								st.nMaxUseNum = ainfo[1];
								st.bAdd=ainfo[2];
								st.nNeedNum = st.nStepNum;
								bFind = true;

								m_aMaterialInfoList.push_back(st);
							}
						}
					}
				}
			}
			break;
		case 2:
			{
				int nNeedLevel = m_MakeData.GetPt_y(m_aMakeObject[i]->dwID,n,pPt);
				for (int j= nNeedLevel;j<=m_aMakeObject[i]->dwChange;j++)
				{
					
					int nItemCount = pMainRole->GetItemCount(m_aMakeObject[i]->dwID,OBJECTCELLTYPE_COMMON,j);
					if (nItemCount > 0)
					{
						stMaterialInfo st;
						st.pObject = m_aMakeObject[i];
						st.ObjectID = m_aMakeObject[i]->dwID;
						st.type = m_aMakeObject[i]->maketype;
						m_MakeData.GetNameAndLevel(st.name,m_aMakeObject[i]->strName,j,st.type,m_aMakeObject[i]->dwChange+m_aMakeObject[i]->dwUp);
						st.nLeaveNum = nItemCount;
						st.nLevel = j;
						st.nUseNum = 0;
						st.nStepNum = ainfo[0];
						st.nMaxUseNum = ainfo[1];
						st.bAdd=1;
						st.nNeedNum = st.nStepNum;
						st.nNeedLevel = nNeedLevel;
						bFind = true;

						m_aMaterialInfoList.push_back(st);
					}
				}
			}
			break;
		}
		
		
		

		if (!bFind&&!IsRateItem(m_aMakeObject[i]->dwID))
		//if (!bFind)
		{
			stMaterialInfo st;
			st.pObject = m_aMakeObject[i];
			st.ObjectID = m_aMakeObject[i]->dwID;
			st.type = m_aMakeObject[i]->maketype;
			st.nLevel = m_MakeData.GetPt_y(st.ObjectID,n,pPt);
			m_MakeData.GetNameAndLevel(st.name,m_aMakeObject[i]->strName,st.nLevel,st.type,m_aMakeObject[i]->dwChange+m_aMakeObject[i]->dwUp);
			st.nLeaveNum = 0;
			st.nUseNum = 0;
			st.nStepNum = ainfo[0];
			st.nMaxUseNum = 0;
			st.bAdd = true;
			st.nNeedNum = st.nStepNum;

			bFind = true;
			m_aMaterialInfoList.push_back(st);
		}

	}

	RefreshData();
	
	m_pMaterial->RemoveAllItems();
	for (int i=0;i<m_aMaterialInfoList.size();i++)
	{
		m_pMaterial->AddItem("",(void*)&(m_aMaterialInfoList[i]));

		ResetListItem(m_pMaterial->GetItemCount()-1);
	}

	FUNCTION_END;
}

/**
 * \brief 判断是否是几率宝石
 * 
 * 判断是否是几率宝石
 * 
 * \param id : 物品ID
 * \return 判断结果
 */
bool CGuiNpcMake::IsRateItem(DWORD id)
{
	FUNCTION_BEGIN;

	bool bRet = false;
	std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,eRateItem),1,"0");
	

	for (int i=0;i<ainfo.size();i++)
	{
		if (ainfo[i]==id)
		{
			bRet = true;
			break;
		}
	}
	return bRet;

	FUNCTION_END;
}

/**
 * \brief 设置打造材料的加减基数
 * 
 * 设置打造材料的加减基数
 * 
 * \return 无
 */
void CGuiNpcMake::RefreshData()
{
	FUNCTION_BEGIN;

	switch(m_eState) {
	case CGuiNpcDialog::eMake:
	//case CGuiNpcDialog::eAddEquipMaklv:	
		{
			for (int i=0;i<m_aMaterialInfoList.size();i++)
			{
				m_aMaterialInfoList[i].nStepNum = 1;
			}
		}
		break;
	}

	FUNCTION_END;
}

/**
* \brief 设置某一项材料的加减按钮
* 
* 设置某一项材料的加减按钮
* 
* \param index : 某一项材料
* \return 无
*/
void CGuiNpcMake::ResetButtonState(int index)
{
	FUNCTION_BEGIN;

	m_aBtnGroupList[index].pButtonAdd->SetEnabled(false);
	m_aBtnGroupList[index].pButtonLess->SetEnabled(false);
	if (m_aBtnGroupList[index].index!=-1)
	{
		stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[index].index);
		if ((pData->nLeaveNum>=pData->nStepNum)&&(pData->nUseNum+pData->nStepNum<=pData->nMaxUseNum))
		{
			m_aBtnGroupList[index].pButtonAdd->SetEnabled(true);
		}
		if (pData->nUseNum>=pData->nStepNum)
		{
			m_aBtnGroupList[index].pButtonLess->SetEnabled(true);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 加载材料列表项
 * 
 * 加载材料列表项
 * 
 * \param index : 材料列表项
 * \return 无
 */
void CGuiNpcMake::ResetListItem(int index)
{
	FUNCTION_BEGIN;

	if ((index<0)||(index>=m_pMaterial->GetItemCount()))
		return;

	
	static int nFixLen = 14;
	char szText[20];
	stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(index);
	
	if (!pData)
		return;

	DWORD color = (GetMaterialData(pData->ObjectID)<pData->nNeedNum)?REDCOLOR:GREENCOLOR;
	
	int nLen=nFixLen-strlen(pData->name);

	nLen = getMax( nLen, 3 );

	m_pMaterial->SetItemText(index,0,pData->name,strlen(pData->name));
	m_pMaterial->SetItemColor(index,0,color);

	if (pData->nNeedNum>=0)
	{
		m_pMaterial->SetItemText(index,1,_itoa(pData->nNeedNum,szText,10),nLen,GuiListBoxColFmt_Right);
		m_pMaterial->SetItemColor(index,1,color);
	}
	else
		m_pMaterial->SetItemText(index,1,"",nLen,GuiListBoxColFmt_Right);
	
	if (pData->nLeaveNum>=0)
	{
		m_pMaterial->SetItemText(index,2,_itoa(pData->nLeaveNum,szText,10),8,GuiListBoxColFmt_Right);
		m_pMaterial->SetItemColor(index,2,color);
	}
	else
		m_pMaterial->SetItemText(index,2,"",8,GuiListBoxColFmt_Right);
	
	if (pData->nUseNum>=0)
	{
		m_pMaterial->SetItemText(index,3,_itoa(pData->nUseNum,szText,10),8,GuiListBoxColFmt_Right);
		m_pMaterial->SetItemColor(index,3,color);
	}
	else
		m_pMaterial->SetItemText(index,3,"",8,GuiListBoxColFmt_Right);

	FUNCTION_END;
}

/**
 * \brief 根据材料ID,等级得到此材料的数量
 * 
 * 根据材料ID,等级得到此材料的数量
 * 
 * \param id : 材料ID
 * \param nLevel : 材料等级
 * \return 材料的数量
 */
int  CGuiNpcMake::GetMaterialData(DWORD id,int nLevel)
{
	FUNCTION_BEGIN;

	int nNum = 0;
	for (int i=0;i<m_aMaterialInfoList.size();i++)
	{
		if (m_aMaterialInfoList[i].ObjectID==id)
		{
			if ((m_aMaterialInfoList[i].nLevel!=nLevel)&&(nLevel!=-1))
				continue;

			nNum += m_aMaterialInfoList[i].nUseNum + m_aMaterialInfoList[i].nLeaveNum;
		}
	}

	return nNum;

	FUNCTION_END;
}


/**
 * \brief 得到合成材料的数量
 * 
 * 得到合成材料的数量
 * 
 * \return 得到合成材料的数量
 */
int  CGuiNpcMake::GetChangeNumBySingle()
{
	return atoi(GetButton(ChangeNum)->GetText());
}

/**
 * \brief 刷新材料加减按钮状态
 * 
 * 刷新材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateGui()
{
	FUNCTION_BEGIN;

	switch(m_eState) {
	case CGuiNpcDialog::eChange:
		UpdateChangeGui();
		break;
	case CGuiNpcDialog::eMake:
		UpdateMakeGui();
		break;
	case CGuiNpcDialog::eUp:
		UpdateUpGui();
		break;
	case CGuiNpcDialog::eKaMake:
		UpdateKaMakeGui();
		break;
	case CGuiNpcDialog::eKaChange:
		UpdateKaChangeGui();
		break;
	case CGuiNpcDialog::eHoleMake:
		UpdateHoleMakeGui();
		break;
	case CGuiNpcDialog::eBackout:
		UpdateBackoutGui();
		break;
	case CGuiNpcDialog::eMakeFood:
	case CGuiNpcDialog::eMakeDrug:
		UpdateMakeFoodGui();
		break;
	//soke 改造装备
    case CGuiNpcDialog::eModifyAttri:           // 改造属性
	case CGuiNpcDialog::eAddReBind:			    // 重新绑定
	case CGuiNpcDialog::eAddReMaker:			// 修改签名
	case CGuiNpcDialog::eAddAddMaker:			// 装备签名
	case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
	case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造
	case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
	case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
	case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
	case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
	{
		UpdateModifyMakeGui();
	}
	break;
	case CGuiNpcDialog::eAddEquipMake:			// 祝福改造
	{
		UpdateMakezsMakeGui();
	}
	break;
	case CGuiNpcDialog::eAddEquipZsMake:		// 马匹祝福改造
	{
		UpdateMakezqMakeGui();
	}
	break;
	case CGuiNpcDialog::eAddEquipMaklv:			// 升级改造
	{
		UpdateMakelvMakeGui();
	}
	break;

	}


	FUNCTION_END;
}

void CGuiNpcMake::ShowMoneyLackInfo()
{
	FUNCTION_BEGIN;

	if ( m_bMoneyOk )
		return;

	switch( m_eState )
	{
	case CGuiNpcDialog::eMake:
		GetGameGuiManager()->AddClientSystemMessage( "银子不足不能打造！" );
		break;
	case CGuiNpcDialog::eHoleMake:
		GetGameGuiManager()->AddClientSystemMessage( "银子不足不能增加凹槽！" );
		break;
	case CGuiNpcDialog::eBackout:
		GetGameGuiManager()->AddClientSystemMessage( "银子不足不能拆除！" );
		break;
	case CGuiNpcDialog::eMakeFood:
	case CGuiNpcDialog::eMakeDrug:
		GetGameGuiManager()->AddClientSystemMessage( "银子不足不能练药！" );
		break;
	}

	FUNCTION_END;
}

/**
* \brief 刷新对话框提示信息
* 
* 刷新对话框提示信息
* 
* \return 无
*/
void CGuiNpcMake::UpdateInfo()
{
	FUNCTION_BEGIN;

	switch(m_eState) {
	case CGuiNpcDialog::eChange:
		UpdateChangeInfo();
		break;
	case CGuiNpcDialog::eMake:
		UpdateMakeInfo();
		break;
	case CGuiNpcDialog::eUp:
		UpdateUpInfo();
		break;
	case CGuiNpcDialog::eKaMake:
		UpdateKaMakeInfo();
		break;
	case CGuiNpcDialog::eKaChange:
		UpdateKaChangeInfo();
		break;
	case CGuiNpcDialog::eHoleMake:
		UpdateHoleMakeInfo();
		break;
	case CGuiNpcDialog::eBackout:
		UpdateBackoutInfo();
		break;
	case CGuiNpcDialog::eMakeFood:
	case CGuiNpcDialog::eMakeDrug:
		UpdateMakeFoodInfo();
		break;
	//soke 改造装备
    case CGuiNpcDialog::eModifyAttri:           // 改造属性
	case CGuiNpcDialog::eAddReBind:			    // 重新绑定
	case CGuiNpcDialog::eAddReMaker:			// 修改签名
	case CGuiNpcDialog::eAddAddMaker:			// 装备签名
	case CGuiNpcDialog::eAddModifySoulA:		// 修改灵魂属性
	case CGuiNpcDialog::eAddLVSoullineMD:		// 保留灵魂锁链改造
	case CGuiNpcDialog::eAddModifyDxSoulA:		// 修改第二灵魂属性
	case CGuiNpcDialog::eAddMDSkillA:			// 修改技能属性
	case CGuiNpcDialog::eAddEquipRound:			// 非转生装备改为转生
	case CGuiNpcDialog::eAddMDMake:			    // 马匹装备重新锻造
	{
		UpdateModifyMakeInfo();
	}
	break;
	case CGuiNpcDialog::eAddEquipMake:			// 祝福改造
	{
		UpdateMakezsMakeInfo();
	}
	break;
	case CGuiNpcDialog::eAddEquipZsMake:		// 马匹祝福改造
	{
		UpdateMakezqMakeInfo();
	}
	break;
	case CGuiNpcDialog::eAddEquipMaklv:			// 升级改造
	{
		UpdateMakelvMakeInfo();
	}
	break;

	}

	FUNCTION_END;
}

/**
 * \brief 刷新合成对话框提示信息
 * 
 * 刷新合成对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateChangeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={CHANGEDEFNEED};
	char szSubTxt[MAX_PATH]={0};
	
	
	InitContext();

	if (m_MakeData.getSelectData().size()==1)
	{
		int price=0;
		if (m_MakeData.getSelectData()[0]->bAdd!=0)
			price=((ObjectBase_t*)(m_MakeData.getSelectData()[0]->pObject))->dwChangePrice;
		else
			price=((CRoleItem*)(m_MakeData.getSelectData()[0]->pObject))->GetObjectBase()->dwChangePrice;

		sprintf(szSubTxt,CHANGEPRICENEED,m_MakeData.getSelectData()[0]->name,price);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);

		int nNum=m_MakeData.getSelectData()[0]->nUseNum/GetChangeNumBySingle();
		int nResidue=m_MakeData.getSelectData()[0]->nUseNum%GetChangeNumBySingle();

		if ((nNum>0)&&(nResidue ==0))
		{
			sprintf(szSubTxt,CHANGEAMTNEED,price*nNum);
			sprintf(szSubTxt,NEEDMONEY,price*nNum);
			m_pClew->SetText(szSubTxt);
			sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);

			m_bMoneyOk = m_MakeData.CheckMoney(price*nNum);
			if ( m_bMoneyOk )
			{
				CanMake();
			}
		}

	}

	
	m_pNeed->SetText("合成高级的原料会打造出更好的装备");


	FUNCTION_END;
}

/**
 * \brief 刷新合成加减按钮状态
 * 
 * 刷新合成加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateChangeGui()
{
	FUNCTION_BEGIN;

	if (m_MakeData.getSelectData().size()==1)
	{
		for (int i=0;i<m_aBtnGroupList.size();i++)
		{
			if (m_aBtnGroupList[i].index!=-1)
			{
				stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[i].index);
				
				if (pData!=m_MakeData.getSelectData()[0])
				{
					m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
					m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
				}
			}
				
		}
	}

	FUNCTION_END;
}

/**
 * \brief 刷新升级对话框提示信息
 * 
 * 刷新升级对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateUpInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={UPDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		DWORD  dwUpID=(pData->nLevel+1)*10000+ ((CRoleItem*)(pData->pObject))->GetObjectBase()->dwID;
		ObjectUp_t* pUp=GetObjectUp(dwUpID);
		if(pUp)
		{
			char szStr[10]={0};
			if (pData->nLevel!=0)
				sprintf(szStr,"+%d",pData->nLevel);

			sprintf(szSubTxt,UPNEED,pUp->dwMoney,szStr);
			
			sprintf(szSubTxt,NEEDMONEY,pUp->dwMoney);
			m_pClew->SetText(szSubTxt);
			sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
		}

		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(UPOBJCET);
		if (pMaterial)
		{
			std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,eRate),
				pData->nLevel+1,"0");
			if (ainfo.size()>0)
			{
				int nRate=ainfo[0];
				pMaterial=m_MakeData.findSelectDataByObjectID(RATEOBJECT);
				if (pMaterial)
				{
					nRate=nRate+pMaterial->nUseNum*UPADD;
				}
				nRate=min(nRate,100);

				sprintf(szSubTxt,UPRATE,nRate);
				m_pNeed->SetText(szSubTxt);
				sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
			}
			
			if (pUp)
			{
				m_bMoneyOk = m_MakeData.CheckMoney(pUp->dwMoney);
				if ( m_bMoneyOk )
				{
					CanMake();
				}
			}

		}
	}
	

	

	FUNCTION_END;
}

/**
 * \brief 刷新升级加减按钮状态
 * 
 * 刷新升级加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateUpGui()
{
	FUNCTION_BEGIN;

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData==NULL)
	{
		for (int i=0;i<m_aBtnGroupList.size();i++)
		{
			if (m_aBtnGroupList[i].index!=-1)
			{
				m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
				m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
			}

		}
	}


	FUNCTION_END;
}

/**
 * \brief 刷新打造对话框提示信息
 * 
 * 刷新打造对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={MAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	m_pNeed->SetVisible(false);

	GetStatic(200)->SetText("");
	GetStatic(201)->SetText("");
	GetStatic(202)->SetText("");
	GetStatic(203)->SetText("");

	GetButton(100)->SetToolTips("白色装备概率");
	GetButton(101)->SetToolTips("蓝色装备概率");
	GetButton(102)->SetToolTips("金色装备概率");
	GetButton(103)->SetToolTips("神圣装备概率");

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		ObjectBase_t* pObject=(ObjectBase_t*)(pData->pObject);
		if(pObject)
		{
			std::vector<UINT, std::stack_allocator<UINT> > aid,maid;
			aid.reserve(16);
			maid.reserve(8);

			_parse_str_num(aid,pObject->strField36);

			std::string  szTitle;
			char        szParse[256]={0};
			bool        bLine=true;
			CRoleSkill*  pSkill=NULL;
            //soke 注释80级以上装备功勋打造
			//if (pObject->needLevel>=80)
			//{
			//	m_bShowExpTitle = true;
			//	if (GetScene()->GetMainCharacter()->GetProData()->exploit<EXPFORMAKE)
			//	{
			//		m_bPass=false;
			//	}
			//}

			if (aid.size()>0)
			{
				m_bPass=true;
				m_bMoneyOk = m_MakeData.CheckMoney(aid[0]);				

				sprintf(szParse,NEEDMONEY,aid[0]);
				m_pClew->SetText(szParse);
				szTitle=szParse;
				szTitle+="\n  所需材料:";
				for (int i=1;i<aid.size();i+=3)
				{
					sprintf(szParse," %s*%d",GetObjectBase(aid[i])->strName,aid[i+2]);
					szTitle+=szParse;

					int nUseNum=m_MakeData.findSelectDataByObjectNum(aid[i]);
					
					if (nUseNum!=aid[i+2])
						m_bPass=false;
				}
			}

			maid[1]=0;
			_parse_str_num(maid,pObject->strField35);

			if (maid.size()>0)
			{
				/*
				
				if (maid[1]==0)
					sprintf(szParse,"\n  所需技能: %s ",GetSkillBase(maid[0])->pstrName);
				else
					sprintf(szParse,"\n  所需技能: %s 等级:%d",GetSkillBase(maid[0])->pstrName,maid[1]);
					*/

				szTitle+=szParse;
				
				if (maid[1]>1)
				{
					pSkill=GetScene()->GetMainCharacter()->FindSkillByID(maid[0]);
					if (!pSkill)
						m_bPass=false;
					else if (pSkill->GetLevel()<maid[1])
						m_bPass=false;
				}

				/*
				if (aid.size()==1)
					sprintf(szParse,"\n  所需技能: %d ",maid[0]);
				else
					sprintf(szParse,"\n  所需技能: %d 等级:%d",maid[0],maid[1]);
				szTitle+=szParse;
				*/

				
			}
			sprintf(szMainTxt,"%s%s",szMainTxt,szTitle.c_str());

			if (m_bPass)
			{
				//m_pNeed->SetTextColor(WHITECOLOR);
				CanMake();

				if (pObject->dwType!=ItemType_Resource)
				{
					//装备几率计算
					byte nRate=0;
					stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(m_MakeData.getMakeRateObject(pObject->dwType));
					if (pMaterial)
					{
						nRate=20;
					}

					m_MakeData.InitOdds();
					int nSkillLevel=1;
					if (pSkill)
						nSkillLevel = pSkill->GetLevel();
					
					m_MakeData.SetOdds(GetScene()->GetMainCharacter()->GetProperty(),nSkillLevel,0);

					/*

					sprintf(szParse,"白色装备概率:%-3.2f%%    蓝色装备概率:%-3.2f%%\n金色装备概率:%-3.2f%%    神圣装备概率:%-3.2f%%\n附加属性概率:%-3.2f%%",
						min((float)m_MakeData.odds_of_white(pObject),100.0f),min((float)((float)m_MakeData.odds_of_blue(pObject)/100),100.0f),
						min((float)((float)m_MakeData.odds_of_gold(pObject)/100),100.0f),min((float)((float)m_MakeData.odds_of_holy(1)/100),100),
						min((float)m_MakeData.odds_of_property(1,1)+nRate,100.0f));
						*/

					
					//m_pNeed->SetText(szParse);


					float odds_white = (float)m_MakeData.odds_of_white(pObject);
					float odds_blue = (float)m_MakeData.odds_of_blue(pObject)/100;
					float odds_gold = (float)m_MakeData.odds_of_gold(pObject)/100;
					float odds_holy = (float)m_MakeData.odds_of_holy(1)/100;

#if defined _DEBUG || defined RELEASE_TEST
					Engine_WriteLogF("白色：%.2f%% 兰色：%.2f%% 金色：%.2f%% 神圣：%.2f%%\n",
						odds_white,odds_blue,odds_gold,odds_holy);
#endif


					odds_white = min(odds_white,100.0f);
					odds_blue = min(odds_blue,100.0f);
					odds_gold = min(odds_gold,100.0f);
					odds_holy = min(odds_holy,100.0f);
					
					odds_holy *= odds_gold/100.0f;
					odds_gold *= ((100.0f-odds_holy)/100.0f);
					odds_blue *= ((100.0f-odds_holy-odds_gold)/100.0f);
					odds_white *= ((100.0f-odds_holy-odds_gold-odds_blue)/100.0f);
					
					
					if (m_MakeData.odds_of_holy(1)/100>100.0f)
						odds_holy = 100.0f - odds_blue-odds_gold-odds_white;
					else if (m_MakeData.odds_of_gold(pObject)/100>100.0f)
						odds_gold = 100.0f - odds_blue-odds_holy-odds_white;
					else if (m_MakeData.odds_of_blue(pObject)/100>100.0f)
						odds_blue = 100.0f - odds_gold-odds_holy-odds_white;
					else if (m_MakeData.odds_of_white(pObject)>100.0f)
						odds_white = 100.0f - odds_blue-odds_gold-odds_holy;

					if (odds_holy<0.0f) odds_holy *= -1.0;
					if (odds_gold<0.0f) odds_gold *= -1.0;
					if (odds_blue<0.0f) odds_blue *= -1.0;
					if (odds_white<0.0f) odds_white *= -1.0;

					//Adjust 2006-02-16
					if ( !((pData->type >= ItemType_ClothBody && pData->type <= ItemType_Fing) || !(pData->type >= ItemType_StaffFlag && pData->type <= ItemType_SwordFlag) )
						&& (pData->type != ItemType_BMW) )
					{
						odds_white = 100.0f;
						odds_blue = 0.0f;
						odds_gold = 0.0f;
						odds_holy = 0.0f;
					}

					

					sprintf(szParse,"%.2f%%",odds_white);
					GetStatic(200)->SetText(szParse);
					sprintf(szParse,"%.2f%%",odds_blue);
					GetStatic(201)->SetText(szParse);
					sprintf(szParse,"%.2f%%",odds_gold);
					GetStatic(202)->SetText(szParse);
					sprintf(szParse,"%.2f%%",odds_holy);
					GetStatic(203)->SetText(szParse);

					
					sprintf(szParse,"生成白色%s的概率是%.2f%%",pData->name, odds_white);
					GetButton(100)->SetToolTips(szParse);
					sprintf(szParse,"生成蓝色%s的概率是%.2f%%",pData->name, odds_blue);
					GetButton(101)->SetToolTips(szParse);
					sprintf(szParse,"生成金色%s的概率是%.2f%%",pData->name, odds_gold);
					GetButton(102)->SetToolTips(szParse);
					sprintf(szParse,"生成神圣%s的概率是%.2f%%",pData->name, odds_holy);
					GetButton(103)->SetToolTips(szParse);

					sprintf(szMainTxt,"%s%s",szMainTxt,szParse);
				}
			}

		}
	}
	

	FUNCTION_END;
}

/**
 * \brief 刷新打造材料加减按钮状态
 * 
 * 刷新打造材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakeGui()
{
	FUNCTION_BEGIN;

	/*
	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		for (int i=0;i<m_aBtnGroupList.size();i++)
		{
			if (m_aBtnGroupList[i].index!=-1)
			{
				stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[i].index);
				stMaterialInfo* pSelectData =m_MakeData.findSelectDataByObjectID(pData->GetObjectBaseID());
				if (pData&&pSelectData)
				{
					if (pData!=pSelectData)
					{
						m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
						m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
					}
				}
			}

		}
	}
	*/

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		for (int i=0;i<m_aBtnGroupList.size();i++)
		{
			if (m_aBtnGroupList[i].index!=-1)
			{
				stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[i].index);
				int nUseNum =m_MakeData.findSelectDataByObjectNum(pData->GetObjectBaseID());
				if (pData&&nUseNum>=pData->nNeedNum)
				{
					m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
					
					//m_aBtnGroupList[i].pButtonLess->SetEnabled(true);
				}
			}

		}
	}
	

	FUNCTION_END;
}

/**
 * \brief 刷新灵魂镶嵌对话框提示信息
 * 
 * 刷新灵魂镶嵌对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateKaMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={KAMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		
		sprintf(szSubTxt,KAMAKEOBJECT,pData->name,
			pData->nCanSocket+pData->nUseSocket,pData->nCanSocket);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
		
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			pData->nUseSocket+1,"16000");
		
		char szStr[10]={0};
		if (pData->nUseSocket!=0)
			sprintf(szStr,"+%d",pData->nUseSocket);

		sprintf(szSubTxt,KAMAKENEED,ainfo[0],szStr);
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
		
		
		
		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(MAKESTONE);
		if (pMaterial)
		{
			
			pMaterial=m_MakeData.findSelectDataByObjectID(KASTONE);
			if (pMaterial)
			{
				m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
				if( m_bMoneyOk )
				{
					CanMake();
					
					CRoleItem* pObject = (CRoleItem*) pData->pObject;
					memcpy(&m_Object,pObject->GetObject(),sizeof(t_Object));
					pObject = (CRoleItem*) pMaterial->pObject;
					do_enchance(&m_Object,pObject->GetObject());

				}
			}
		}
		
	}


	//m_pNeed->SetText(szMainTxt);

	FUNCTION_END;
}

/**
 * \brief 刷新魂魄镶嵌材料加减按钮状态
 * 
 * 刷新魂魄镶嵌材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateKaMakeGui()
{
	FUNCTION_BEGIN;

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData==NULL)
		UpdateUpGui();
	else
		UpdateMakeGui();

	FUNCTION_END;
}

/**
 * \brief 刷新灵魂合成对话框提示信息
 * 
 * 刷新灵魂合成对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateKaChangeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={KACHANGEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	
	InitContext();
	
	if (m_MakeData.findSelectDataByObjectNum(KASTONE)==2)
	{
		int nMaxLevel=m_MakeData.getSelectDataByObjectMaxLevel(KASTONE);
		DWORD dwMoney=GetObjectBase(KASTONE)->dwChangePrice;
		
		sprintf(szSubTxt,NEEDMONEY,dwMoney);
		m_pClew->SetText(szSubTxt);

		char szStr[10]={0};
		if (nMaxLevel!=0)
			sprintf(szStr,"+%d",nMaxLevel);

		sprintf(szSubTxt,KACHANGENEED,dwMoney,szStr);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
		
		int nStone=m_MakeData.findSelectDataByObjectNum(RATESTONE);
		switch(nStone) {
		case 1:
			sprintf(szSubTxt,KACHANGERATE,70);
			break;
		case 2:
			sprintf(szSubTxt,KACHANGERATE,100);
			break;
		default:
			sprintf(szSubTxt,"");
			break;
		}
		m_pNeed->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);
		if (nStone>=1)
		{
			m_bMoneyOk = m_MakeData.CheckMoney(dwMoney);
			if ( m_bMoneyOk )
			{
				CanMake();
				
				t_Object *object1,*object2;
				int n=0;
				for (int i=0;i<3;i++)
				{
					stMaterialInfo* p=m_MakeData.getSelectData()[i];
					if (!p)
						break;

					if (p->GetObjectBaseID()==KASTONE)
					{
						if (n==0)
						{
							object1=((CRoleItem*)p->pObject)->GetObject();
						}
						else
							object2=((CRoleItem*)p->pObject)->GetObject();

						n++;
					}
				}
				memcpy(&m_Object,object1,sizeof(t_Object));
				do_compose(object1,object2,&m_Object);

			}
		}

	}


	//m_pNeed->SetText(szMainTxt);

	FUNCTION_END;
}

/**
 * \brief 刷新魂魄合成材料加减按钮状态
 * 
 * 刷新魂魄合成材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateKaChangeGui()
{
	FUNCTION_BEGIN;

	int nStone=m_MakeData.findSelectDataByObjectNum(KASTONE);
	int nMaxLevel=m_MakeData.getSelectDataByObjectMaxLevel(KASTONE);

	stPropInfo* pPropData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pPropData==NULL)
		return;


	for (int i=0;i<m_aBtnGroupList.size();i++)
	{
		if (m_aBtnGroupList[i].index!=-1)
		{
			stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[i].index);
			
			if (!pData)
				continue;

			switch(nStone) {
			case 2:
				{
					if (m_MakeData.findSelectDataByData(pData))
					{
						if ((m_MakeData.findSelectDataByObjectID(RATESTONE)!=NULL)&&pData->GetObjectBaseID()==KASTONE)
						{
							m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
							m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
						}
					}
					else
					{
						if (pData->GetObjectBaseID()==KASTONE)
						{
							m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
							m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
						}
						else
						{
							if (nMaxLevel!=pData->nLevel)
							{
								m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
								m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
							}
						}
					}
				}
				break;
			case 0:
				{
					if (pData->GetObjectBaseID()==KASTONE)
					{
						if (pData->nLevel!=pPropData->nLevel-1)
						{
							m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
							m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
						}
					}
				}
			case 1:
				{
					if (pData->GetObjectBaseID()!=KASTONE)
					{
						m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
						m_aBtnGroupList[i].pButtonLess->SetEnabled(false);
					}
				}
				break;
			
			}
		}

	}
	

	FUNCTION_END;
}

/**
 * \brief 刷新物品改造对话框提示信息
 * 
 * 刷新物品改造对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateModifyMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={HOLEMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			1,"32000");
		ainfo[0]=10000;//改造价格

		char szStr[10]={0};

		sprintf(szSubTxt,HOLEMAKENEED,ainfo[0],szStr);
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);


		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(MODIFYSTONE);
		if (pMaterial)
		{	
			m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
			CanMake();
		}

		//m_pNeed->SetText("此装备使用改造之星进行改造,以加强装备的属性");

	}

	FUNCTION_END;
}

/**
 * \brief 刷新改造材料加减按钮状态
 * 
 * 刷新改造材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateModifyMakeGui()
{
	FUNCTION_BEGIN;

	UpdateUpGui();

	FUNCTION_END;
}

/**
 * \brief 刷新物品祝福改造对话框提示信息
 * 
 * 刷新物品改造对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakezsMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={HOLEMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			1,"32000");
		ainfo[0]=500000;//神圣祝福价格

		char szStr[10]={0};

		sprintf(szSubTxt,HOLEMAKENEED,ainfo[0],szStr);
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);


		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(MAKEZSTONE);
		if (pMaterial)
		{	
			m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
			CanMake();
		}

		//m_pNeed->SetText("此装备使用改造之星进行改造,以加强装备的属性");

	}

	FUNCTION_END;
}

/**
 * \brief 刷新物品祝福改造对话框提示信息
 * 
 * 刷新物品改造对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakezqMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={HOLEMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			1,"32000");
		ainfo[0]=500000;//神圣祝福价格

		char szStr[10]={0};

		sprintf(szSubTxt,HOLEMAKENEED,ainfo[0],szStr);
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);


		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(MAKEZQTONE);
		if (pMaterial)
		{	
			m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
			CanMake();
		}

		//m_pNeed->SetText("此装备使用改造之星进行改造,以加强装备的属性");

	}

	FUNCTION_END;
}

/**
 * \brief 刷新物品升级改造对话框提示信息
 * 
 * 刷新物品改造对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakelvMakeInfo()
{
	FUNCTION_BEGIN;

	InitContext();
	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
	if (pData)
	{
		ObjectBase_t* pObject=((CRoleItem*)pData->pObject)->GetObjectBase();
		if (pObject)
		{
			std::vector<UINT, std::stack_allocator<UINT> > aid;
			aid.reserve(16);
			_parse_str_num(aid,pObject->strField36);

			
			if (aid.size()>0)
			{
				m_bPass=true;
				m_bMoneyOk = m_MakeData.CheckMoney(400000);
				char szSubTxt[MAX_PATH]={0};
				sprintf(szSubTxt,NEEDMONEY,400000);
				m_pClew->SetText(szSubTxt);
				int nUseNum=m_MakeData.findSelectDataByObjectNum(MAKELVTONE);
				if (nUseNum!=MAKELVNUM)
					m_bPass=false;
				if(pObject->needLevel > 160)
				{
					int index = aid.size() -3;
					nUseNum = 0;
					nUseNum = m_MakeData.findSelectDataByObjectNum(aid[index]);
					if (nUseNum!=aid[index+2])
						m_bPass=false;
				}
			}
		}

		if (m_bPass)
			CanMake();

	}


		//-------------------s

/* 		ObjectBase_t* pObject=(ObjectBase_t*)(pData->pObject);
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			1,"32000");
		ainfo[0]=400000;//装备升级价格

		char szStr[10]={0};
		
		//sprintf(szSubTxt,HOLEMAKENEED,ainfo[0],szStr);
		
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);


		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(MAKELVTONE);
		if(pObject->needLevel >= 197)
		{
			if (pMaterial)
			{	
				m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
				CanMake();
			}
		}else
		{
			WORD id = 0;
			if(pObject->needLevel == 197 )
				id=2901;
			else if(pObject->needLevel == 202)
				id=2902;
			else if(pObject->needLevel == 205)
				id=2903;
			else if(pObject->needLevel == 210)
				id=2904;
			if(id > 0)
			{
				stMaterialInfo* pMaterial2=m_MakeData.findSelectDataByObjectID(id);
				if (pMaterial&&pMaterial2)
				{	
					m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
					CanMake();
				}
			} 
			
		}*/
		

		//m_pNeed->SetText("此装备使用改造之星进行改造,以加强装备的属性");


	FUNCTION_END;
}

/**
 * \brief 刷新祝福改造材料加减按钮状态
 * 
 * 刷新改造材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakezsMakeGui()
{
	FUNCTION_BEGIN;

	UpdateUpGui();

	FUNCTION_END;
}

/**
 * \brief 刷新祝福改造材料加减按钮状态
 * 
 * 刷新改造材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakezqMakeGui()
{
	FUNCTION_BEGIN;

	UpdateUpGui();

	FUNCTION_END;
}

/**
 * \brief 刷新升级改造材料加减按钮状态
 * 
 * 刷新改造材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakelvMakeGui()
{
	FUNCTION_BEGIN;

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		for (int i=0;i<m_aBtnGroupList.size();i++)
		{
			if (m_aBtnGroupList[i].index!=-1)
			{
				stMaterialInfo* pData = (stMaterialInfo*) m_pMaterial->GetItemData(m_aBtnGroupList[i].index);
				int nUseNum =m_MakeData.findSelectDataByObjectNum(pData->GetObjectBaseID());
				if (pData&&nUseNum>=pData->nNeedNum)
				{
					m_aBtnGroupList[i].pButtonAdd->SetEnabled(false);
					
					//m_aBtnGroupList[i].pButtonLess->SetEnabled(true);
				}
			}

		}
	}

	FUNCTION_END;
}

/**
 * \brief 刷新物品打孔对话框提示信息
 * 
 * 刷新物品打孔对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateHoleMakeInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={HOLEMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		int nSocket=pData->nUseSocket+pData->nCanSocket;
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(m_eState,ePrice),
			nSocket+1,"32000");

		char szStr[10]={0};
		if (nSocket!=0)
			sprintf(szStr,"+%d",nSocket);

		sprintf(szSubTxt,HOLEMAKENEED,ainfo[0],szStr);
		sprintf(szSubTxt,NEEDMONEY,ainfo[0]);
		m_pClew->SetText(szSubTxt);
		sprintf(szMainTxt,"%s%s",szMainTxt,szSubTxt);


		stMaterialInfo* pMaterial=m_MakeData.findSelectDataByObjectID(HOLESTONE);
		if (pMaterial)
		{	
			m_bMoneyOk = m_MakeData.CheckMoney(ainfo[0]);
			CanMake();
		}

		m_pNeed->SetText("此装备增加凹槽后可镶嵌一颗魂魄,以加强装备的属性");

	}


	

	FUNCTION_END;
}


/**
 * \brief 刷新物品打孔材料加减按钮状态
 * 
 * 刷新物品打孔材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateHoleMakeGui()
{
	FUNCTION_BEGIN;

	UpdateUpGui();

	FUNCTION_END;
}

/**
 * \brief 刷新拆除物品对话框提示信息
 * 
 * 刷新拆除物品对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateBackoutInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={HOLEMAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());
	
	if (pData)
	{
		CRoleItem*  pRoleItem = (CRoleItem*)pData->pObject;
		
		if (pRoleItem==NULL)
			return;

		int nMoney = m_MakeData.GetBackoutMoney(pRoleItem);
		sprintf(szSubTxt,NEEDMONEY,nMoney);
		m_pClew->SetText(szSubTxt);
		
		m_bMoneyOk = m_MakeData.CheckMoney(nMoney);
		{
			CanMake();
			float nItemRate = 0.0f;
			float nStoneRate = 0.0f;
			

			if (pRoleItem->GetObject()->kind&0x4)
			{
				nItemRate = 50.0f;
				nStoneRate = 6.0f*((float)pRoleItem->GetObjectBase()->needLevel/20.0f+1.0f);
			}
			else if (pRoleItem->GetObject()->kind&0x2)
			{
				nItemRate = 5.0f;
				nStoneRate = 1.0f*((float)pRoleItem->GetObjectBase()->needLevel/20.0f+1.0f);
			}
			else if (pRoleItem->GetObject()->kind&0x1)
			{
				nItemRate = 5.0f;
				nStoneRate = 0.5f*((float)pRoleItem->GetObjectBase()->needLevel/20.0f+1.0f);
			}
			else
			{
				m_pMake->GetToolTips()->AddText("拆除当前物品得不到任何东西");
				m_pMake->GetToolTips()->Resize();
			}


			//sprintf(szMainTxt,"此装备得到高级原料概率%3.2f%%\n此装备得到宝石概率%3.2f%%",
			//	nItemRate,nStoneRate);
			sprintf(szMainTxt,"拆除此装备有几率得到高级原料");
			m_pNeed->SetText(szMainTxt);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 刷新拆除物品材料加减按钮状态
 * 
 * 刷新拆除物品材料加减按钮状态
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateBackoutGui()
{
	FUNCTION_BEGIN;
	
	for (int i=0;i<m_aBtnGroupList.size();i++)
	{
		if (m_aBtnGroupList[i].index!=-1)
		{
			m_aBtnGroupList[i].pButtonAdd->SetVisible(false);
			m_aBtnGroupList[i].pButtonLess->SetVisible(false);
		}

	}

	FUNCTION_END;
}

/**
 * \brief 刷新食物烹饪对话框提示信息
 * 
 * 刷新食物烹饪对话框提示信息
 * 
 * \return 无
 */
void CGuiNpcMake::UpdateMakeFoodInfo()
{
	FUNCTION_BEGIN;

	char szMainTxt[MAX_PATH]={MAKEDEFNEED};
	char szSubTxt[MAX_PATH]={0};

	InitContext();

	m_pNeed->SetVisible(false);


	stPropInfo* pData = (stPropInfo*)m_pProp->GetItemData(m_pProp->GetCurItem());

	if (pData)
	{
		ObjectBase_t* pObject=(ObjectBase_t*)(pData->pObject);
		if(pObject)
		{
			std::vector<UINT, std::stack_allocator<UINT> > aid,maid;
			aid.reserve(16);
			maid.reserve(8);

			_parse_str_num(aid,pObject->strField36);

			std::string  szTitle;
			char        szParse[256]={0};
			bool        bLine=true;
			CRoleSkill*  pSkill=NULL;

			if (aid.size()>0)
			{
				m_bPass=true;
				m_bMoneyOk = m_MakeData.CheckMoney(aid[0]);
				
				sprintf(szParse,NEEDMONEY,aid[0]);
				m_pClew->SetText(szParse);
				szTitle=szParse;
				szTitle+="\n  所需材料:";
				for (int i=1;i<aid.size();i+=3)
				{
					sprintf(szParse," %s*%d",GetObjectBase(aid[i])->strName,aid[i+2]);
					szTitle+=szParse;

					int nUseNum=m_MakeData.findSelectDataByObjectNum(aid[i]);

					if (nUseNum!=aid[i+2])
						m_bPass=false;
				}
			}

			maid[1]=0;
			_parse_str_num(maid,pObject->strField35);

			if (maid.size()>0)
			{

				/*if (maid[1]==0)
					sprintf(szParse,"\n  所需技能: %s ",GetSkillBase(maid[0])->pstrName);
				else
					sprintf(szParse,"\n  所需技能: %s 等级:%d",GetSkillBase(maid[0])->pstrName,maid[1]);
				szTitle+=szParse;*/

				if (maid[1]>1)
				{
					pSkill=GetScene()->GetMainCharacter()->FindSkillByID(maid[0]);
					if (!pSkill)
						m_bPass=false;
					else if (pSkill->GetLevel()<maid[1])
						m_bPass=false;
				}

				/*
				if (aid.size()==1)
				sprintf(szParse,"\n  所需技能: %d ",maid[0]);
				else
				sprintf(szParse,"\n  所需技能: %d 等级:%d",maid[0],maid[1]);
				szTitle+=szParse;
				*/


			}
			sprintf(szMainTxt,"%s%s",szMainTxt,szTitle.c_str());

			if (m_bPass)
			{
				//m_pNeed->SetTextColor(WHITECOLOR);
				CanMake();

			}

		}
	}


	FUNCTION_END;
}

/**
 * \brief 刷新食物烹饪材料加减按钮状态
 * 
 * 刷新食物烹饪材料加减按钮状态
 * 
 * \return 返回值的描述
 */
void CGuiNpcMake::UpdateMakeFoodGui()
{
	FUNCTION_BEGIN;

	UpdateMakeGui();

	FUNCTION_END;
}

/**
 * \brief 初始化对话框信息
 * 
 * 初始化对话框信息
 * 
 * \return 无
 */
void CGuiNpcMake::InitContext()
{
	FUNCTION_BEGIN;

	m_bPass=false;
	m_bMoneyOk = false;
	
	//m_MoneyColor = REDCOLOR;
	m_pNeed->SetTextColor(YELLOWCOLOR);
	m_pNeed->SetText("");
	m_pClew->SetText("0");
	m_pMake->SetEnabled(false);

	m_pMake->GetToolTips()->Clear();

	m_bShowExpTitle = false;

	FUNCTION_END;
}

/**
 * \brief 满足打造条件的处理
 * 
 * 满足打造条件的处理
 * 
 * \return 无
 */
void CGuiNpcMake::CanMake()
{
	FUNCTION_BEGIN;

	m_bPass=true;
	//m_pNeed->SetTextColor(WHITECOLOR);
	m_pMake->SetEnabled(true);

	//m_MoneyColor = GREENCOLOR;

	FUNCTION_END;
}

//月光宝盒
CGuiMakeTo::CGuiMakeTo(CRoleItem* pRoleItem)
{
	m_bCloseIsHide = true;
	InitTableInfo( pRoleItem );
}

void CGuiMakeTo::InitTableInfo( CRoleItem* pRoleItem )
{
	m_pRoleItem = pRoleItem;
	m_nPos = m_pRoleItem->GetThisID();
	m_nLocationPos = m_pRoleItem->GetLocation().x;
}

void CGuiMakeTo::OnClose(void)
{
	if (!m_bCloseIsHide)
	{
		switch(m_nLocationPos) 
		{
		case EQUIPCELLTYPE_LEFT:
			GetGameGuiManager()->m_guiMakeTo1 = NULL;
			break;
		case EQUIPCELLTYPE_RIGHT:
			GetGameGuiManager()->m_guiMakeTo2 = NULL;
			break;
		}
	}
}

void CGuiMakeTo::RefreshTableInfo()
{
	m_pEquipTable = GetTable(100);
	m_pEquipTable->m_iTableType = OBJECTCELLTYPE_MAKE;
	m_pEquipTable->m_iTableID = m_nPos;
	m_pEquipTable->m_EquipPosition = stPointI(0,MAKECELLTYPE_EQUIP);

	m_pMateroalTable1 = GetTable(101);
	m_pMateroalTable1->m_iTableType = OBJECTCELLTYPE_MAKE;
	m_pMateroalTable1->m_iTableID = m_nPos;
	m_pMateroalTable1->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL1);

	m_pMateroalTable2 = GetTable(102);
	m_pMateroalTable2->m_iTableType = OBJECTCELLTYPE_MAKE;
	m_pMateroalTable2->m_iTableID = m_nPos;
	m_pMateroalTable2->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL2);

	m_pMateroalTable3 = GetTable(103);
	m_pMateroalTable3->m_iTableType = OBJECTCELLTYPE_MAKE;
	m_pMateroalTable3->m_iTableID = m_nPos;
	m_pMateroalTable3->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL3);

	m_pMateroalTable4 = GetTable(104);
	m_pMateroalTable4->m_iTableType = OBJECTCELLTYPE_MAKE;
	m_pMateroalTable4->m_iTableID = m_nPos;
	m_pMateroalTable4->m_EquipPosition = stPointI(0,MAKECELLTYPE_MATERIAL4);
}

//创建窗口
void CGuiMakeTo::OnCreate()
{
	CGuiDialog::OnCreate();

	m_BackElements.resize(1);

	CGuiElement* pEmt = new CGuiElement;
	pEmt->FontColor.Current = YELLOWCOLOR;
	pEmt->iFont = 1;
	pEmt->TextureColor.Current = COLOR_ARGB(128,0,0,0);
	pEmt->dwTextFormat = DT_BOTTOM | DT_CENTER;
	m_BackElements[0] = pEmt;

	m_pMake = GetButton(204);

	RefreshTableInfo();

	GetButton(201)->SetButtonGroup(1);
	GetButton(202)->SetButtonGroup(1);
	GetButton(203)->SetButtonGroup(1);
	GetButton(200)->SetButtonGroup(1);

	m_pClew = GetStatic(ClewLabel);
	m_pNeed = GetStatic(NeedLabel);
	m_pContext = GetStatic(5);
	m_pClew->SetVisible(false);

	GetButton(ChangeNum)->SetText("4");

	m_pTab = GetTab(3);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);

	m_pTab->AddControl(0,GetButton(ChangeNum));
	m_pTab->AddControl(0,GetStatic(4));

	m_pTab->AddControl(1,GetButton(21));
	m_pTab->AddControl(1,GetComboBox(22));

	CGuiComboBox* pCombo = GetComboBox(22);
	if(pCombo)
	{
		pCombo->RemoveAllItems();
		for (int i = 15; i > 0; i--)
		{
			char szStr[256];
			sprintf(szStr, "冲到%d星", i);
			pCombo->AddItem(szStr, NULL);
		}
	}

	m_autoUpdateUp = false;
	m_autolevel = 15;
	m_isUpdateUp = false;
	
	m_pTab->SetVisible(false);

	
	OnGuiEvent(EVENT_BUTTON_CLICKED,200,NULL);

}

HRESULT CGuiMakeTo::OnRender(float fElapsedTime)
{
	HRESULT  hr = CGuiDialog::OnRender(fElapsedTime);
	
	GetButton( 999 )->SetLocation( 354, 115 );//月光关闭按钮

	m_aniMake.Render(m_x+3,m_y-3);//这里是升星绑定镶嵌合成失败特效地址

	if(m_autoUpdateUp)
	{
		if(m_isUpdateUp == false)
		{
			m_isUpdateUp = true;
			AotuUpdateUp();
		}
	}else
	{
		if(m_isUpdateUp)
		{
			m_isUpdateUp = false;
		}
		if(strcmp(GetButton(21)->GetText(),"停止冲星") == 0)
		{
			GetButton(21)->SetText("自动冲星");
		}
	}
	

	int nMoney = strtol(m_pClew->GetText(),NULL,10);

	if (nMoney<=0)
		return hr;

	nMoney += CCountry::Instance()->GetRevenue(nMoney);

	if (m_MakeData.CheckMoney(nMoney))
		m_MoneyColor = GREENCOLOR;
	else	
		m_MoneyColor = REDCOLOR;

	m_BackElements[0]->FontColor.Current =  m_MoneyColor;
	DrawText(Money2String(atoi(m_pClew->GetText())),m_BackElements[0],&(stRectI(160,150,300,166)));

	return hr;
}

void CGuiMakeTo::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x-5,pt.y+26);
	}

	FUNCTION_END;
}

bool CGuiMakeTo::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{	
	switch(nEvent) 
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) 
			{
			case 999:	
			{
				m_autoUpdateUp = false;
            	SetVisible(false);
			}
			break;
			case 200:
			case 201:
			case 202:
			case 203:
				GetButton(nID)->SetChecked(true);
				m_pTab->SetCurItem(nID-200);
				UpdateData(true);
				break;
			case ChangeNum:
				{
					switch(GetChangeNumBySingle()) {
					case 5:
						//GetButton(ChangeNum)->GetToolTips()->Clear();
						//GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数3\n合成成功率50%");
						//GetButton(ChangeNum)->GetToolTips()->Resize();
						GetButton(ChangeNum)->SetText("3");
						m_pNeed->SetText("成功率50%");
						break;
					case 4:
						GetButton(ChangeNum)->SetText("5");
						m_pNeed->SetText("成功率100%");
						//GetButton(ChangeNum)->GetToolTips()->Clear();
						//GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数5\n合成成功率100%");
						//GetButton(ChangeNum)->GetToolTips()->Resize();
						break;
					case 3:
						GetButton(ChangeNum)->SetText("4");
						m_pNeed->SetText("成功率75%");
						//GetButton(ChangeNum)->GetToolTips()->Clear();
						//GetButton(ChangeNum)->GetToolTips()->AddText("单次合成基数4\n合成成功率75%");
						//GetButton(ChangeNum)->GetToolTips()->Resize();
						break;
					}
				}
				UpdateData(true);
				break;
			case 21:
				{
					if(m_autoUpdateUp)
					{
						m_autoUpdateUp = false;
					}else
					{
						m_autoUpdateUp = true;
					}
				}
				break;
			case 204:
				{
					if (GetGuiManager()->GetFloatItem())
					{
						GameMessageBox("请放下鼠标上的物品后再使用月光宝合!");
						break;
					}
					if (UpdateData(true))
					{
						SendCmd();
						g_nPos = m_nPos;
					}
				}
				break;
			}
		}
		break;
	case EVENT_COMBOBOX_SELECTION_CHANGED:
		{
			m_autolevel = 15-GetComboBox(22)->GetCurItem();
			Engine_WriteLogF("当前:冲到%d星 \n",m_autolevel);
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

/**
* \brief 得到合成材料的数量
* 
* 得到合成材料的数量
* 
* \return 得到合成材料的数量
*/
int  CGuiMakeTo::GetChangeNumBySingle()
{
	return atoi(GetButton(ChangeNum)->GetText());
}

void CGuiMakeTo::SetMakeParam(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	m_aMakeObject.clear();
	if( params.size() > 0 )
	{
		ParseMakeString(m_aMakeObject,params[0].c_str(),m_eState);
	}


	FUNCTION_END;
}

void CGuiMakeTo::InsertItemData(CGuiTable* pTable)
{
	if (pTable)
	{
		if (pTable->m_listItem.size()<=0)
			return;

		for (int i=0;i<pTable->m_listItem.size();i++)
		{
			if ((pTable->m_listItem[i]->m_pItemData!=NULL)&&(pTable->m_listItem[i]->IsEnabled()))
				m_aMateroalObject.push_back((CRoleItem*)pTable->m_listItem[i]->m_pItemData);
		}
	}
}

void CGuiMakeTo::FlashItemData()
{
	m_aMateroalObject.clear();

	InsertItemData(m_pEquipTable);
	InsertItemData(m_pMateroalTable1);
	InsertItemData(m_pMateroalTable2);
	InsertItemData(m_pMateroalTable3);
	InsertItemData(m_pMateroalTable4);
}

void CGuiMakeTo::InitData()
{
	switch(m_pTab->GetCurItem()) {
	case 0:
		InitChange();
		break;
	case 1:
		InitUp();
		break;
	case 2:
		InitKaChange();
		break;
	case 3:
		InitKaMake();
		break;
	}
}

bool CGuiMakeTo::UpdateData(bool bShowContext)
{
	if (m_pRoleItem->GetObject()->dur==0)
	{
		m_pClew->SetText("0");

		m_pNeed->SetText("");
		m_pContext->SetText("使用次数已经用完，无法再使用！");

		GetButton(ChangeNum)->SetEnabled(false);
		
		GetButton(204)->SetEnabled(false);
		Engine_WriteLogF("结束 \n");
		return false;
	}
	else
	{
		GetButton(ChangeNum)->SetEnabled(true);

		GetButton(204)->SetEnabled(true);
	}

	FlashItemData();

	InitData();

	switch(m_pTab->GetCurItem()) {
	case 0:
		return UpdateChange(bShowContext);
	case 1:
		return UpdateUp(bShowContext);
	case 2:
		return UpdateKaChange(bShowContext);
	case 3:
		return UpdateKaMake(bShowContext);
	}
	return false;
}

void  CGuiMakeTo::InitChange()
{
	m_pClew->SetText("0");
	switch(GetChangeNumBySingle()) {
	case 5:
		m_pNeed->SetText("成功率100%");
		break;
	case 4:
		m_pNeed->SetText("成功率75%");
		break;
	case 3:
		m_pNeed->SetText("成功率50%");
		break;
	}
	m_pContext->SetText("");

	std::vector<std::string> params;
	params.push_back(m_MakeData.GetIni()->GetString("1","change",""));
	SetMakeParam(params);
}

bool  CGuiMakeTo::UpdateChange(bool bShowContext)
{

	if (m_aMateroalObject.size()<=0)
		return false;

	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		int bFind = false;
		for (int j=0;j<m_aMakeObject.size();j++)
		{
			if (m_aMateroalObject[i]->GetObjectID()==m_aMakeObject[j]->dwID)
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			SetContext("对不起，非原料类物品不能合成！",bShowContext);
			return false;
		}
	}
	
	int nLevel = m_aMateroalObject[0]->GetObject()->upgrade;
	int dwID = m_aMateroalObject[0]->GetObjectID();
	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if ((nLevel != m_aMateroalObject[i]->GetObject()->upgrade)||(dwID != m_aMateroalObject[i]->GetObjectID()))
		{
			SetContext("相同物品相同等级才能进行合成！",bShowContext);
			return false;
		}
	}
	
	int nCount = 0;
	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		nCount += m_aMateroalObject[i]->GetCount();
	}
	if (nCount % GetChangeNumBySingle() != 0)
	{
		SetContext("原料的数量必须为合成基数的整数倍才能合成！",bShowContext);
		return false;
	}
	
	if ((nLevel == m_aMateroalObject[0]->GetObjectBase()->dwChange)&&(m_aMateroalObject[0]->GetObjectBase()->dwMake==0))
	{
		SetContext("当前原料已经达到最大等级,无法再合成！",bShowContext);
		return false;
	}

	int nMoney = (int)(nCount / GetChangeNumBySingle()) * m_aMateroalObject[0]->GetObjectBase()->dwChangePrice;
	sprintf(m_szSubTxt,NEEDMONEY,nMoney);
	m_pClew->SetText(m_szSubTxt);
	if (m_MakeData.CheckMoney(nMoney)==false)
	{
		SetContext("银子不足，不能合成！",bShowContext);
		return false;
	}

	return true;
	
}

void  CGuiMakeTo::InitUp()
{
	m_pClew->SetText("0");

	m_pNeed->SetText("");
	m_pContext->SetText("单件装备配合对应的升级宝石可升级！");

	

}

bool  CGuiMakeTo::UpdateUp(bool bShowContext)
{
	CRoleItem* pItemUp=NULL;
	int nItemUp = 0,nItemChange = 0,nItemChange2 = 0,nItemChange3= 0,nItemRate = 0,nItemXing = 0,nItemHsf = 0;

	if (m_aMateroalObject.size()<=0)
		return false;

	
	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if (m_aMateroalObject[i]->GetObjectID()==UPOBJCET) // 升级宝石 681
		{
			nItemChange++;
			continue;
		}
		if (m_aMateroalObject[i]->GetObjectID()==UPOBJCETBETTER) // 精致升级宝石 795
		{
			nItemChange2++;
			continue;
		}

		if (m_aMateroalObject[i]->GetObjectID()==UPOBJCETWUXIA) //无暇升级宝石 799
		{
			nItemChange3++;
			continue;
		}
		if (m_aMateroalObject[i]->GetObjectID() >= 2960 && m_aMateroalObject[i]->GetObjectID() <= 2974)
		{
			nItemXing++;
			continue;
		}
		if (m_aMateroalObject[i]->GetObjectID() == 34567)//护身符碎片
		{
			nItemHsf++;
			continue;
		}
		//m_aMateroalObject[i]->GetObjectBase()->dwUp 强制设置为 201
		if ((m_aMateroalObject[i]->GetObjectBase()->dwUp>0)&&(201 > m_aMateroalObject[i]->GetObject()->upgrade))
		{
			nItemUp++;
			pItemUp = m_aMateroalObject[i];
			continue;
		}

		SetContext("存在非升级类道具！",bShowContext);
		return false;
	}

	int count = 0;
    if (nItemUp != 0) count++;
    if (nItemChange != 0) count++;
    if (nItemXing != 0) count++;
	if (nItemHsf != 0) count++;
    if (nItemChange2 != 0) count++;
    if (nItemChange3 != 0) count++;


	if (count > 2)
	{
		SetContext("存在多种宝石!",bShowContext);
		return false;
	}

	if ((nItemUp+nItemChange+nItemXing+nItemHsf+nItemChange2+nItemChange3)<m_aMateroalObject.size())
	{
		SetContext("存在非升级类道具！",bShowContext);
		return false;
	}
	
	if (nItemUp==1) //装备
	{
		if (pItemUp->GetObject()->needlevel<40)
		{
		   if( (pItemUp->GetObject()->kind & 16) || !(pItemUp->GetObject()->kind & 4) )
		   {
		      if (nItemChange==0)
			  {
			     SetContext("40级以下的神圣祝福装备需要升级宝石来升级！",bShowContext);
				 return false;
			  }
		   }
		   else
		   {
		      if (nItemChange==0)
			  {
			     SetContext("40级以下的装备需要升级宝石来升级！",bShowContext);
				 return false;
			  }
		   }
		}
		else
		{
	       if( (pItemUp->GetObject()->kind & 16) && !( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 ) )
		   {
		      if (nItemChange3==0 && nItemXing == 0)
			  {
				SetContext("40级及以上的神圣祝福装备需要无暇致升级宝石来升级！",bShowContext);
				return false;
			  }
		   }
		   else if( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 )
		   {
		      if (nItemHsf==0)
			  {
				SetContext("60级及以上的护身符需要护身符碎片来升级！",bShowContext);
				return false;
			  }
		   }
		   else if( !(pItemUp->GetObject()->kind & 16) )
		   {
			  if (nItemChange2==0 && nItemXing == 0)
			  {
				SetContext("40级及以上的装备需要精致升级宝石来升级！",bShowContext);
				return false;
			  }
		  }
		}
	}

	if ((nItemUp!=1)||((nItemChange + nItemChange2 + nItemChange3 + nItemXing + nItemHsf)<1))
	{
		SetContext("单件装备与最少一个升级宝石才能升级！",bShowContext);
		return false;
	}

	if (pItemUp->GetObject()->needlevel<40)
	{
		if (nItemChange2>0)
		{
			SetContext("40级以下的装备需要升级宝石来升级！",bShowContext);
			return false;
		}
		nItemRate = nItemChange - 1;
	}
	else
	{
	  if( (pItemUp->GetObject()->kind & 16) && !( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 ))
	  {
		if (nItemChange>0 && nItemChange2>0)
		{
			SetContext("40级及以上的神圣祝福装备需要无暇升级宝石来升级！",bShowContext);
			return false;
		}
		nItemRate = nItemChange3 - 1;
	  }
	  else if( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 )
	  {
		 if (nItemChange>0 && nItemChange2>0  && nItemChange3>0)
		 {
			SetContext("60级及以上的护身符需要护身符碎片来升级！",bShowContext);
			return false;
		 }
	  }
	  else if( !(pItemUp->GetObject()->kind & 16) )
	  {
		if (nItemChange>0 && nItemChange3>0)
		{
			SetContext("40级及以上的装备需要精致升级宝石来升级！",bShowContext);
			return false;
		}
		nItemRate = (int)(nItemChange2+nItemChange3) - 1;
	  }
	}
	
	
	//if (nItemRate>1)
	//{
	//	SetContext("最多只能放一个天佑石！",bShowContext);
	//	return false;
	//}

	//if (pItemUp->GetObjectBase()->dwUp==pItemUp->GetObject()->upgrade)
	if (pItemUp->GetObject()->dwObjectID == 18005 || pItemUp->GetObject()->dwObjectID == 18010 || pItemUp->GetObject()->dwObjectID == 18015 || pItemUp->GetObject()->dwObjectID == 18020 || pItemUp->GetObject()->dwObjectID == 18025)
	{
	   if (200 == pItemUp->GetObject()->upgrade)    //将护身符最大升级等级直接设置为201
	   {
		   SetContext("护身符等级已经最高，不能再升级！",bShowContext);
		   return false;
	   }
	}
	else
	{
		
	   /* if (15 == pItemUp->GetObject()->upgrade && !( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 ))    //将最大升级等级直接设置为15星
	   {
		   SetContext("当前等级已经最高，不能再升级！",bShowContext);
		   return false;
	   } */
	   if(!( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 ))
	   {
			if(m_autoUpdateUp)
			{
				if(pItemUp->GetObject()->upgrade >= m_autolevel)
				{
					SetContext("当前星级已到达自动升星等级!",bShowContext);
					return false;
				}
			}
			if (15 == pItemUp->GetObject()->upgrade)
			{
				SetContext("当前等级已经最高，不能再升级！",bShowContext);
				return false;
			}
	   }
	}

	DWORD  dwUpID=(pItemUp->GetObject()->upgrade+1) * 100000 + pItemUp->GetObjectID();
	
	if(dwUpID >= 1600000)
	{
		ObjectUp_t m_pUp;

		m_pUp.dwID = dwUpID;
		m_pUp.dwMoney = 5000;

		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(CGuiNpcDialog::eUp,eRate),
			pItemUp->GetObject()->upgrade+1,"0");

		int nRate=ainfo[0];
		nRate+=nItemRate*UPADD;

		if(nItemChange3 > 0)
		{
			nRate += 20;
			sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		}
		else
		{
			sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		}
		m_pNeed->SetText(m_szSubTxt);

		sprintf(m_szSubTxt,NEEDMONEY,m_pUp.dwMoney);

		m_pClew->SetText(m_szSubTxt);

		if (m_MakeData.CheckMoney(m_pUp.dwMoney)==false)
		{
			SetContext("对不起，您银子不足，不能升级！",bShowContext);
			if ( bShowContext )
				GetGameGuiManager()->AddClientSystemMessage( "对不起，您银子不足，不能升级！" );
			return false;
		}

		return true;
	}

	ObjectUp_t* pUp=GetObjectUp(dwUpID);

	if(pUp)
	{
		std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(CGuiNpcDialog::eUp,eRate),
			pItemUp->GetObject()->upgrade+1,"0");

		int nRate=ainfo[0];
		nRate+=nItemRate*UPADD;

        if( (pItemUp->GetObject()->kind & 16) && !( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 ))
		{
		   if(nItemChange3 > 0)
		   {
			  sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		   }
		}
		//soke 20177-1-22 护身符
		else if( pItemUp->GetObject()->dwObjectID >= 18001 && pItemUp->GetObject()->dwObjectID <= 18025 )
		{
		   if(nItemHsf > 0)
		   {
			  sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		   }
		}
		else if( !(pItemUp->GetObject()->kind & 16) )
		{
		   if(nItemChange3 > 0)
		   {
		      nRate += 20;
			  sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		   }
		   else
		   {
			  sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		   }
		}
		else
		{
		    sprintf(m_szSubTxt,"成功率%d%%",min(nRate,100));
		}
		if (nItemXing != 0)
			m_pNeed->SetText("成功率100%");
		else
			m_pNeed->SetText(m_szSubTxt);

		sprintf(m_szSubTxt,NEEDMONEY,pUp->dwMoney);
		m_pClew->SetText(m_szSubTxt);

		if (m_MakeData.CheckMoney(pUp->dwMoney)==false)
		{
			SetContext("对不起，您银子不足，不能升级！",bShowContext);
			if ( bShowContext )
				GetGameGuiManager()->AddClientSystemMessage( "对不起，您银子不足，不能升级！" );
			return false;
		}
	}
	else
		return false;

	return true;
}

void CGuiMakeTo::AotuUpdateUp()
{	
		GetButton(21)->SetText("停止冲星");
		if (GetGuiManager()->GetFloatItem())
		{
			m_isUpdateUp = false;
			m_autoUpdateUp = false;
			GameMessageBox("请放下鼠标上的物品后再使用月光宝合!");
			return;
		}

		if (UpdateData(true))
		{
			SendCmd();
			g_nPos = m_nPos;
		}else
		{
			m_isUpdateUp = false;
			m_autoUpdateUp = false;
		}
}

void  CGuiMakeTo::InitKaChange()
{
	m_pClew->SetText("0");

	m_pContext->SetText("带空凹槽的装备可镶嵌魂魄石！");

	m_pNeed->SetText("");

	std::vector<std::string> params;
	params.push_back("id[680,748,749,750,751]");
	SetMakeParam(params);
}

bool  CGuiMakeTo::UpdateKaChange(bool bShowContext)
{
	CRoleItem* pItemKaChange=NULL;
	int nItemKa = 0,nItemChange = 0,nItemKaChange = 0; //定魂石
	WORD  nUseSocket = 0,nCanSocket = 0;


	if (m_aMateroalObject.size()<=0)
		return false;

	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if (m_aMateroalObject[i]->GetObjectID()==MAKESTONE)
		{
			nItemChange++;
			continue;
		}
		
		WORD nUseSocket_t = 0;
		WORD nCanSocket_t = m_MakeData.CheckKaMake(m_aMateroalObject[i]->GetObject()->socket,nUseSocket_t);
		if (nCanSocket_t>0)
		{
			nItemKa++;
			nCanSocket = nCanSocket_t;
			nUseSocket = nUseSocket_t;
			pItemKaChange = m_aMateroalObject[i];
			continue;
		}

		int bFind = false;
		for (int j=0;j<m_aMakeObject.size();j++)
		{
			if (m_aMateroalObject[i]->GetObjectID()==m_aMakeObject[j]->dwID)
			{
				nItemKaChange++;
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			SetContext("存在非镶嵌类道具！",bShowContext);
			return false;
		}
	}

	if ((nItemKa+nItemChange+nItemKaChange)<m_aMateroalObject.size())
	{
		SetContext("存在非镶嵌类道具！",bShowContext);
		return false;
	}

	if ((nItemKa!=1)||(nItemKaChange!=1)||(nItemChange!=1))
	{
		SetContext("对不起，单件空凹槽装备配合单个魂魄石和单个定魂石才能镶嵌！",bShowContext);
		return false;
	}

	/*if (nCanSocket == nUseSocket)
	{
		SetContext("对不起，此装备已经没有可用的凹槽！",bShowContext);
		return false;
	}*/

	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if (m_aMateroalObject[i]->GetObjectID()==MAKESTONE)
		{
			if (m_aMateroalObject[i]->GetObject()->upgrade != nUseSocket)
			{
				char szStr[16]={0};
				//if (nUseSocket!=0)
					sprintf(szStr,"%d等",nUseSocket+1);
				sprintf(m_szSubTxt,"对不起，镶嵌此装备需要[定魂石 %s]一个！",szStr);
				SetContext(m_szSubTxt,bShowContext);
				return false;
			}
			else
				break;

		}
	}


	std::vector<UINT, std::stack_allocator<UINT> >&  ainfo = m_MakeData.GetInfo(m_MakeData.GetMainItem(CGuiNpcDialog::eKaMake,ePrice),
		nUseSocket+1,"16000");

	sprintf(m_szSubTxt,NEEDMONEY,ainfo[0]);
	m_pClew->SetText(m_szSubTxt);

	
	if (m_MakeData.CheckMoney(ainfo[0])==false)
	{
		SetContext("对不起，您银子不足，不能镶嵌！",bShowContext);
		if ( bShowContext )
			GetGameGuiManager()->AddClientSystemMessage( "对不起，您银子不足，不能镶嵌!" );
		return false;
	}

	return true;
}
// 桃子修复灵魂宝石月光宝盒可绑定装备
void  CGuiMakeTo::InitKaMake()
{
	m_pClew->SetText("0");

	m_pContext->SetText("装备物品用灵魂宝石可实现与自身灵魂的绑定！");

	m_pNeed->SetText("");

	std::vector<std::string> params;
	params.push_back("type[101-118,130-134,136-138,141-147,155-156]");
	SetMakeParam(params);
}

bool  CGuiMakeTo::UpdateKaMake(bool bShowContext)
{
	CRoleItem* pItemKa=NULL;
	int nItemKa = 0,nItemChange = 0;
	
	if (m_aMateroalObject.size()<=0)
		return false;

	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if (m_aMateroalObject[i]->GetObjectID()==KAMAKESTONE)
		{
			nItemChange++;
			continue;
		}

		int bFind = false;
		for (int j=0;j<m_aMakeObject.size();j++)
		{
			if (m_aMateroalObject[i]->GetObjectID()==m_aMakeObject[j]->dwID)
			{
				nItemKa++;
				pItemKa = m_aMateroalObject[i];

				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			SetContext("对不起，非装备类物品不能进行灵魂绑定！",bShowContext);
			return false;
		}
	}

	if ((nItemKa+nItemChange)<m_aMateroalObject.size())
	{
		SetContext("对不起，非装备类物品不能合成进行灵魂绑定！",bShowContext);
		return false;
	}

	if ((nItemKa!=1)||(nItemChange!=1))
	{
		SetContext("必须放一个装备物品和一个灵魂宝石进行灵魂绑定！",bShowContext);
		return false;
	}

	if (pItemKa->IsBind())
	{
		SetContext("已经绑定的装备物品不能再次绑定！",bShowContext);
		return false;
	}

	m_pNeed->SetText("成功率100%");

	/*if (nItemKaChange!=2)
	{
		SetContext("对不起，请使用两个魂魄石进行合成！",bShowContext);
		return false;
	}

	int nLevel = max(pItemKaChange1->GetObject()->upgrade,pItemKaChange2->GetObject()->upgrade);

	for (int i=0;i<m_aMateroalObject.size();i++)
	{
		if (m_aMateroalObject[i]->GetObjectID()==RATESTONE)
		{
			if (m_aMateroalObject[i]->GetObject()->upgrade != nLevel )
			{
				SetContext("对不起，存在不满足合成的凝魂石！",bShowContext);
				return false;
			}
			else
			{
				continue;
			}
		}
	}*/
	

	//switch(nItemChange) {
	//case 0:
	//	{
	//		/*char szStr[16]={0};
	//		if (nLevel!=0)
	//			sprintf(szStr,"%d等",nLevel);
	//		sprintf(m_szSubTxt,"对不起，缺少%s凝魂石一个！",szStr);*/
	//		SetContext("对不起，缺少女娲彩石一个！",bShowContext);
	//		return false;
	//	}
	//	break;
	//case 1:
	//	{
	//		
	//	}
	//	break;
	//default:
	//	{
	//		SetContext("对不起，最多只能放一个女娲彩石进行绑定！",bShowContext);
	//		return false;
	//	}
	//}


	int nMoney=200;
	sprintf(m_szSubTxt,NEEDMONEY,nMoney);
	m_pClew->SetText(m_szSubTxt);


	if (m_MakeData.CheckMoney(nMoney)==false)
	{
		SetContext("对不起，您银子不足，不能绑定！",bShowContext);
		if ( bShowContext )
			GetGameGuiManager()->AddClientSystemMessage( "对不起，您银子不足，不能绑定!" );
		return false;
	}

	return true;
}

void CGuiMakeTo::Response(stResponsePropertyUserCmd* pCmd)
{
	if (pCmd->status==0)
	{
		switch(m_pTab->GetCurItem()) 
		{
		case 0:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 50;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10005);
			}
			break;
		case 1:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 52;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10008);
			}
			break;
		case 2:
			{	
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 51;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10007);
			}
			break;
		case 3:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 53;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
				PlayUISound(10006);
			}
			break;
		}
	}
	else
	{

		switch(m_pTab->GetCurItem()) 
		{
		case 0:
		case 1:
		case 2:
			{	
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 55;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
			}
			break;
		case 3:
			{
				stResourceLocation rl;
				rl.SetFileName("data\\interfaces.gl");

				rl.group = 31;
				rl.frame = 54;
				m_aniMake.Create(&rl);
				m_aniMake.SetLoopPlay(false);
			}
			break;
		}
		PlayUISound(10009);
	}
	InitData();
}

int  CGuiMakeTo::GetItemCount()
{
	FlashItemData();
	return m_aMateroalObject.size();
}

bool CGuiMakeTo::SendCmd()
{
	switch(m_pTab->GetCurItem()) 
	{
	case 0:
		{
			stComposeItemPropertyUserCmd cmd;
			cmd.per_num = GetChangeNumBySingle();
			cmd.location = m_nLocationPos;
			SEND_USER_CMD(cmd);
			
			return true;
		}
		break;
	case 1:
		{
			stUpgradeItemPropertyUserCmd cmd;
			cmd.location = m_nLocationPos;
			SEND_USER_CMD(cmd);
			return true;
		}
		break;
	case 2:
		{
			stEnchasePropertyUserCmd  cmd;
			cmd.location = m_nLocationPos;
			SEND_USER_CMD(cmd);
			return true;
		}
		break;
	case 3:
		{
			stComposeSoulStonePropertyUserCmd cmd;
			cmd.location = m_nLocationPos;
			SEND_USER_CMD(cmd);
			return true;
		}
		break;
	}

	return false;
}