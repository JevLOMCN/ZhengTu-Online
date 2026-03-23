/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    经脉对话框
*
* 
*
*/
#include "public.h"
#include "../gui/include/GuiManager.h"	
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./Guishengxiao.h"
#include "./table.h"
#include "./RoleItem.h"
#include "Maincharacter.h"
#include "./GameScene.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_JIANZHONG  200    // 剑冢
#define ID_BUTTON_JINGMAI    201    // 经脉
#define ID_BUTTON_SHENGXIAO  202   // 生肖
#define ID_BUTTON_SHENGQI    203    // 圣器
#define ID_BUTTON_ZHENHGTUZHUAN  204 // 征途传
#define ID_BUTTON_TISHENG     29 // 提升
 
#define ID_BUTTON_CLOSE       1   //关闭
///////////////////////////////////////////////////////////////////////////////
CGuishengxiaoDlg::CGuishengxiaoDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuishengxiaoDlg::~CGuishengxiaoDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuishengxiaoDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	select = 0; // 给select一个初始值 0
	sx1=0;
	sx2=0;
	sx3=0;
	sx4=0;
	sx5=0;
	sx6=0;
	sx7=0;
	sx8=0;
	sx9=0;
	sx10=0;
	sx11=0;
	sx12=0;

	shengx1 = GetButton(60);
	shengx2 = GetButton(61);
	shengx3 = GetButton(62);
	shengx4 = GetButton(63);
	shengx5 = GetButton(64);
	shengx6 = GetButton(65);
	shengx7 = GetButton(66);
	shengx8 = GetButton(67);
	shengx9 = GetButton(68);
	shengx10 = GetButton(69);
	shengx11 = GetButton(70);
	shengx12 = GetButton(71);
	wenzi = GetStatic(30);

	cltext = GetStatic(27);
	moneytext = GetStatic(28);

	m_pTableMaterial1 = this->GetTable(130);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,3);

	FUNCTION_END;
}

bool CGuishengxiaoDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		// char msg[256];
		// sprintf(msg,"您正在按的按钮为：%d",nID);
		// GetGameGuiManager()->AddClientSystemMessage(msg);
		switch(nID)
		{
		case ID_BUTTON_CLOSE:  // 关闭
			{
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JINGMAI:  // 经脉
			{
				GetGameGuiManager()->Addjingmai();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JIANZHONG:  // 剑冢
			{
				GetGameGuiManager()->Addjianzhong();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_SHENGXIAO:  // 生肖
			{
				
			}
			break;
		case ID_BUTTON_SHENGQI:  // 圣器
			{
				GetGameGuiManager()->Addshengqi();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_ZHENHGTUZHUAN:  // 征途转
			{
				GetGameGuiManager()->Addzhengtuzhuan();
				SetVisible(false);
			}
			break;
			case 60:
			{
				char a[32];
				select = 1;
				this->update(sx1);
				if(sx1==0)
				{
					wenzi->SetText("十二生肖-子鼠尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-子鼠 当前等级：%d",sx1);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 61:
			{
				char a[32];
				select = 2;
				this->update(sx2);
				if(sx2==0)
				{
					wenzi->SetText("十二生肖-丑牛尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-丑牛 当前等级：%d",sx2);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 62:
			{
				char a[32];
				select = 3;
				this->update(sx3);
				if(sx3==0)
				{
					wenzi->SetText("十二生肖-寅虎尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-寅虎 当前等级：%d",sx3);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 63:
			{
				char a[32];
				select = 4;
				this->update(sx4);
				if(sx4==0)
				{
					wenzi->SetText("十二生肖-卯兔尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-卯兔 当前等级：%d",sx4);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 64:
			{
				char a[32];
				select = 5;
				this->update(sx5);
				if(sx5==0)
				{
					wenzi->SetText("十二生肖-辰龙尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-辰龙 当前等级：%d",sx5);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 65:
			{
				char a[32];
				select = 6;
				this->update(sx6);
				if(sx6==0)
				{
					wenzi->SetText("十二生肖-巳蛇尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-巳蛇 当前等级：%d",sx6);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 66:
			{
				char a[32];
				select = 7;
				this->update(sx7);
				if(sx7==0)
				{
					wenzi->SetText("十二生肖-午马尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-午马 当前等级：%d",sx7);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 67:
			{
				char a[32];
				select = 8;
				this->update(sx8);
				if(sx8==0)
				{
					wenzi->SetText("十二生肖-未龙尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-未龙 当前等级：%d",sx8);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 68:
			{
				char a[32];
				select = 9;
				this->update(sx9);
				if(sx9==0)
				{
					wenzi->SetText("十二生肖-申猴尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-申猴 当前等级：%d",sx9);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 69:
			{
				char a[32];
				select = 10;
				this->update(sx10);
				if(sx10==0)
				{
					wenzi->SetText("十二生肖-酉鸡尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-酉鸡 当前等级：%d",sx10);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 70:
			{
				char a[32];
				select = 11;
				this->update(sx11);
				if(sx11==0)
				{
					wenzi->SetText("十二生肖-戌狗尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-戌狗 当前等级：%d",sx11);
			        wenzi->SetText(a); 				
				}				
			}
			break;
			case 71:
			{
				char a[32];
				select = 12;
				this->update(sx12);
				if(sx12==0)
				{
					wenzi->SetText("十二生肖-亥猪尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-亥猪 当前等级：%d",sx12);
			        wenzi->SetText(a); 				
				}				
			}
			break;

			case 29:
			{
				if(GetScene()->GetMainCharacter()->GetLevel()<170)
				{
					cltext->SetText("170级开放征途转功能！");
					return false;
				}
				if(select==0)
				{
					return false;
				}
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem) 
				{
					return false;
				}
				stFujianShengxiaoLevelup cmd;
				cmd.shengx=select; 
			    cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			    SEND_USER_CMD(cmd);
			}
			break;
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

CRoleItem* CGuishengxiaoDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

void CGuishengxiaoDlg::update(int num)
{
	CGuiStatic *level = GetStatic(51);
	char a[256];
	sprintf(a, "%d级", num);
	level->SetText(a);
	CGuiStatic *level2 = GetStatic(54);
	sprintf(a, "%d级", num + 1);
	level2->SetText(a);

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	shengxiaopeizhi sjs;
	for(int i=0;i<num;i++)
	{
		sjs = g_shengxiao[i]; 
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}

	char nums[256];
	sprintf(nums,"+%d",pDam);
	GetStatic(311)->SetText(nums);
	sprintf(nums,"+%d",mDam);
	GetStatic(312)->SetText(nums);
	sprintf(nums,"+%d",pDef);
	GetStatic(313)->SetText(nums);
	sprintf(nums,"+%d",mDef);
	GetStatic(314)->SetText(nums);
	sprintf(nums,"+%d",hp);
	GetStatic(315)->SetText(nums);

	//下一级
	shengxiaopeizhi zt = g_shengxiao[num];
	sprintf(nums,"+%d",zt.pDam);
	GetStatic(321)->SetText(nums);
	sprintf(nums,"+%d",zt.mDam);
	GetStatic(322)->SetText(nums);
	sprintf(nums,"+%d",zt.pDef);
	GetStatic(323)->SetText(nums);
	sprintf(nums,"+%d",zt.mDef);
	GetStatic(324)->SetText(nums);
	sprintf(nums,"+%d",zt.hp);
	GetStatic(325)->SetText(nums);

	int money=zt.salary;

	int ding =0;
	int liang =0;
	char moneytexts[32];
	if(money>10000){
		ding = money/10000;
		liang = (money-(ding*10000))/100;
		sprintf(moneytexts,"             需要银子:%d锭%d两",ding,liang);
	}
	else{
		liang = money/100;
		sprintf(moneytexts,"             需要银子:%d两",liang);
	}
	if(num<100)
	{
		moneytext->SetText(moneytexts);
		ObjectBase_t* pObject = GetObjectBase(zt.costID);
		char cailiao[256];
		sprintf(cailiao,"           需要%s 数量:%d",pObject->strName,zt.costNum);
		cltext->SetText(cailiao);
	}

}

void CGuishengxiaoDlg::SHUAXIN()
{
	if(sx1>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx1->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,5), 1);
		if(sx1>0 && sx1<=20)
		{
			CGuiImage *img = GetImage(1010);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx1>20 && sx1<=40)
		{
			CGuiImage *img = GetImage(1110);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx1>40 && sx1<=60)
		{
			CGuiImage *img = GetImage(1210);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx1>60 && sx1<=80)
		{
			CGuiImage *img = GetImage(1310);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx1>80)
		{
			CGuiImage *img = GetImage(1410);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx2>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx2->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,6), 1);
		if(sx2>0 && sx2<=20)
		{
			CGuiImage *img = GetImage(1011);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx2>20 && sx2<=40)
		{
			CGuiImage *img = GetImage(1111);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx2>40 && sx2<=60)
		{
			CGuiImage *img = GetImage(1211);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx2>60 && sx2<=80)
		{
			CGuiImage *img = GetImage(1311);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx2>80)
		{
			CGuiImage *img = GetImage(1411);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx3>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx3->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,7), 1);
		if(sx3>0 && sx3<=20)
		{
			CGuiImage *img = GetImage(1012);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx3>20 && sx3<=40)
		{
			CGuiImage *img = GetImage(1112);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx3>40 && sx3<=60)
		{
			CGuiImage *img = GetImage(1212);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx3>60 && sx3<=80)
		{
			CGuiImage *img = GetImage(1312);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx3>80)
		{
			CGuiImage *img = GetImage(1412);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx4>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx4->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,8), 1);
		if(sx4>0 && sx4<=20)
		{
			CGuiImage *img = GetImage(1013);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx4>20 && sx4<=40)
		{
			CGuiImage *img = GetImage(1113);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx4>40 && sx4<=60)
		{
			CGuiImage *img = GetImage(1213);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx4>60 && sx4<=80)
		{
			CGuiImage *img = GetImage(1313);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx4>80)
		{
			CGuiImage *img = GetImage(1413);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx5>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx5->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,9), 1);
		if(sx5>0 && sx5<=20)
		{
			CGuiImage *img = GetImage(1014);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx5>20 && sx5<=40)
		{
			CGuiImage *img = GetImage(1114);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx5>40 && sx5<=60)
		{
			CGuiImage *img = GetImage(1214);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx5>60 && sx5<=80)
		{
			CGuiImage *img = GetImage(1314);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx5>80)
		{
			CGuiImage *img = GetImage(1414);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx6>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx6->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,10), 1);
		if(sx6>0 && sx6<=20)
		{
			CGuiImage *img = GetImage(1015);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx6>20 && sx6<=40)
		{
			CGuiImage *img = GetImage(1115);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx6>40 && sx6<=60)
		{
			CGuiImage *img = GetImage(1215);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx6>60 && sx6<=80)
		{
			CGuiImage *img = GetImage(1315);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx6>80)
		{
			CGuiImage *img = GetImage(1415);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx7>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx7->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,11), 1);
		if(sx7>0 && sx7<=20)
		{
			CGuiImage *img = GetImage(1016);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx7>20 && sx7<=40)
		{
			CGuiImage *img = GetImage(1116);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx7>40 && sx7<=60)
		{
			CGuiImage *img = GetImage(1216);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx7>60 && sx7<=80)
		{
			CGuiImage *img = GetImage(1316);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx7>80)
		{
			CGuiImage *img = GetImage(1416);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx8>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx8->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,12), 1);
		if(sx8>0 && sx8<=20)
		{
			CGuiImage *img = GetImage(1017);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx8>20 && sx8<=40)
		{
			CGuiImage *img = GetImage(1117);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx8>40 && sx8<=60)
		{
			CGuiImage *img = GetImage(1217);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx8>60 && sx8<=80)
		{
			CGuiImage *img = GetImage(1317);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx8>80)
		{
			CGuiImage *img = GetImage(1417);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx9>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx9->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,13), 1);
		if(sx9>0 && sx9<=20)
		{
			CGuiImage *img = GetImage(1018);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx9>20 && sx9<=40)
		{
			CGuiImage *img = GetImage(1118);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx9>40 && sx9<=60)
		{
			CGuiImage *img = GetImage(1218);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx9>60 && sx9<=80)
		{
			CGuiImage *img = GetImage(1318);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx9>80)
		{
			CGuiImage *img = GetImage(1418);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx10>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx10->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,14), 1);
		if(sx10>0 && sx10<=20)
		{
			CGuiImage *img = GetImage(1019);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx10>20 && sx10<=40)
		{
			CGuiImage *img = GetImage(1119);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx10>40 && sx10<=60)
		{
			CGuiImage *img = GetImage(1219);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx10>60 && sx10<=80)
		{
			CGuiImage *img = GetImage(1319);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx10>80)
		{
			CGuiImage *img = GetImage(1419);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx11>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx11->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,15), 1);
		if(sx11>0 && sx11<=20)
		{
			CGuiImage *img = GetImage(1020);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx11>20 && sx11<=40)
		{
			CGuiImage *img = GetImage(1120);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx11>40 && sx11<=60)
		{
			CGuiImage *img = GetImage(1220);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx11>60 && sx1<=80)
		{
			CGuiImage *img = GetImage(1320);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx11>80)
		{
			CGuiImage *img = GetImage(1420);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}
	if(sx12>0)
	{
		//SetBitmap 是CGuiButton下的方法
		shengx12->SetBitmap(stResourceLocation("data\\interfaces3.gl",4,16), 1);
		if(sx12>0 && sx12<=20)
		{
			CGuiImage *img = GetImage(1021);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,300));
		}
		if(sx12>20 && sx12<=40)
		{
			CGuiImage *img = GetImage(1121);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,301));
		}
		if(sx12>40 && sx12<=60)
		{
			CGuiImage *img = GetImage(1221);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,302));
		}	
		if(sx12>60 && sx12<=80)
		{
			CGuiImage *img = GetImage(1321);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,303));
		}
		if(sx12>80)
		{
			CGuiImage *img = GetImage(1421);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",4,304));
		}
	}

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	shengxiaopeizhi sjs;

	for(int i=0;i<sx1;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx2;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx3;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx4;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx5;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx6;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx7;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx8;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx9;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx10;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx11;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<sx12;i++)
	{
		sjs = g_shengxiao[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}

	char num[256];
	
	sprintf(num,"+%d",pDam);
	GetStatic(331)->SetText(num);
	sprintf(num,"+%d",mDam);
	GetStatic(332)->SetText(num);
	sprintf(num,"+%d",pDef);
	GetStatic(333)->SetText(num);
	sprintf(num,"+%d",mDef);
	GetStatic(334)->SetText(num);
	sprintf(num,"+%d",hp);
	GetStatic(335)->SetText(num);

	char a[32];
	switch(select)
	{
		case 1:
		{
			this->update(sx1);
				if(sx1==0)
				{
					wenzi->SetText("十二生肖-子鼠尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-子鼠 当前等级：%d",sx1);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 2:
		{
			this->update(sx2);
				if(sx2==0)
				{
					wenzi->SetText("十二生肖-丑牛尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-丑牛 当前等级：%d",sx2);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 3:
		{
			this->update(sx3);
				if(sx3==0)
				{
					wenzi->SetText("十二生肖-寅虎尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-寅虎 当前等级：%d",sx3);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 4:
		{
			this->update(sx4);
				if(sx4==0)
				{
					wenzi->SetText("十二生肖-卯兔尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-卯兔 当前等级：%d",sx4);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 5:
		{
			this->update(sx5);
				if(sx5==0)
				{
					wenzi->SetText("十二生肖-辰龙尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-辰龙 当前等级：%d",sx5);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 6:
		{
			this->update(sx6);
				if(sx6==0)
				{
					wenzi->SetText("十二生肖-巳蛇尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-巳蛇 当前等级：%d",sx6);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 7:
		{
			this->update(sx7);
				if(sx7==0)
				{
					wenzi->SetText("十二生肖-午马尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-午马 当前等级：%d",sx7);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 8:
		{
			this->update(sx8);
				if(sx8==0)
				{
					wenzi->SetText("十二生肖-未羊尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-未羊 当前等级：%d",sx8);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 9:
		{
			this->update(sx9);
				if(sx9==0)
				{
					wenzi->SetText("十二生肖-申猴尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-申猴 当前等级：%d",sx9);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 10:
		{
			this->update(sx10);
				if(sx10==0)
				{
					wenzi->SetText("十二生肖-酉鸡尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-酉鸡 当前等级：%d",sx10);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 11:
		{
			this->update(sx11);
				if(sx11==0)
				{
					wenzi->SetText("十二生肖-戌狗尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-戌狗 当前等级：%d",sx11);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 12:
		{
			this->update(sx12);
				if(sx12==0)
				{
					wenzi->SetText("十二生肖-亥猪尚未激活");
				}
				else
				{
			        sprintf(a,"十二生肖-亥猪 当前等级：%d",sx12);
			        wenzi->SetText(a); 				
				}
		}
		break;
	}

}

void CGuishengxiaoDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guishengxiao = NULL;

	FUNCTION_END;
}