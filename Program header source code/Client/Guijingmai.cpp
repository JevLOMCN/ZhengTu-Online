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
#include "./Guijingmai.h"
#include "./table.h"
#include "./RoleItem.h"

///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_JIANZHONG  200    // 剑冢
#define ID_BUTTON_JINGMAI    201    // 经脉
#define ID_BUTTON_SHENGXIAO  202   // 生肖
#define ID_BUTTON_SHENGQI    203    // 圣器
#define ID_BUTTON_ZHENHGTUZHUAN  204 // 征途传
#define ID_BUTTON_TISHENG     29 // 提升
 
#define ID_BUTTON_CLOSE       1   //关闭


#define JINGMAI1       100   //关闭
#define JINGMAI2       101  //关闭
#define JINGMAI3       102   //关闭

///////////////////////////////////////////////////////////////////////////////
CGuijingmaiDlg::CGuijingmaiDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuijingmaiDlg::~CGuijingmaiDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuijingmaiDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

    jm1=0;
	jm2=0;
	jm3=0;
	//好了 可以写了 数据库存三个 jing123 其他的一样
	select=1;
	jingm1=GetButton(JINGMAI1);
	jingm2=GetButton(JINGMAI2);
	jingm3=GetButton(JINGMAI3);
	jingmaibg=GetImage(11);
	jingmairenwu=GetImage(13);
	dian1 = GetImage(50);
	dian2 = GetImage(51);
	dian3 = GetImage(52);
	dian4 = GetImage(53);
	dian5 = GetImage(54);
	dian6 = GetImage(55);
	dian7 = GetImage(56);
	dian8 = GetImage(57);
	dian9 = GetImage(58);
	dian10 = GetImage(59);
	dian11 = GetImage(117);
	quanbj2 = GetImage(15);

	if(select==1)
	{
		jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,5),1);
		jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,6),1);
		jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,9),1);
	}
	else if(select==2)
	{
		jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,3),1);
		jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,8),1);
		jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,9),1);		
	}
	else
	{
		jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,3),1);
		jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,6),1);
		jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,11),1);
	}

	
	wenzi = GetStatic(104);

	cltext = GetStatic(27);
	moneytext = GetStatic(28);
	
	m_pTableMaterial1 = this->GetTable(130);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,2);

	FUNCTION_END;
}


bool CGuijingmaiDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_CLOSE:  // 关闭
			{
				SetVisible(false);
			}
			break;
		case ID_BUTTON_JINGMAI:  // 经脉
			{
				
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
				GetGameGuiManager()->Addshengxiao();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_SHENGQI:  // 圣器
			{
				GetGameGuiManager()->Addshengqi();
				SetVisible(false);
			}
			break;
		case ID_BUTTON_ZHENHGTUZHUAN:  //征途传 
			{
				GetGameGuiManager()->Addzhengtuzhuan();
				SetVisible(false);
			}
			break;
		case JINGMAI1:  //经脉1 
			{
				select=1;
				jingmaibg->SetImage(stResourceLocation("data\\interfaces3.gl",1,0));
				jingmairenwu->SetImage(stResourceLocation("data\\interfaces3.gl",1,17));
				jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,5),1);
				jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,6),1);
				jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,9),1);
				dian1->SetLocation(415,134);
				dian2->SetLocation(414,176);
				dian3->SetLocation(365,214);
				dian4->SetLocation(472,217);
				dian5->SetLocation(449,267);
				dian6->SetLocation(354,323);
				dian7->SetLocation(399,379);
				dian8->SetLocation(482,379);
				dian9->SetLocation(520,343);
				dian10->SetLocation(436,327);
				wenzi->SetText("任脉：");
				dian11->SetLocation(411,130);//这个 小黄点 第一个坐标 经脉1
				this->update(jm1);
				//十个 找出来写到这
			}
			break;
		case JINGMAI2:  //经脉2 
			{
				select=2;
				jingmaibg->SetImage(stResourceLocation("data\\interfaces3.gl",1,1));
				jingmairenwu->SetImage(stResourceLocation("data\\interfaces3.gl",1,18));
				jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,3),1);
				jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,8),1);
				jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,9),1);
				dian1->SetLocation(382,158);
				dian2->SetLocation(334,192);
				dian3->SetLocation(415,193);
				dian4->SetLocation(370,223);
				dian5->SetLocation(375,260);
				dian6->SetLocation(308,314);
				dian7->SetLocation(428,293);
				dian8->SetLocation(381,348);
				dian9->SetLocation(432,363);
				dian10->SetLocation(471,444);
				dian11->SetLocation(378,154);
				wenzi->SetText("督脉：");
				this->update(jm2);
				//十个找出来写到这 以此类推
				
			}
			break;
		case JINGMAI3:  //经脉3 
			{
				select=3;
				jingmaibg->SetImage(stResourceLocation("data\\interfaces3.gl",1,2));
				jingmairenwu->SetImage(stResourceLocation("data\\interfaces3.gl",1,19));
				jingm1->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,3),1);
				jingm2->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,6),1);
				jingm3->SetBitmap(stResourceLocation("data\\interfaces3.gl",1,11),1);
				dian1->SetLocation(421,165);
				dian2->SetLocation(441,193);
				dian3->SetLocation(391,206);
				dian4->SetLocation(426,239);
				dian5->SetLocation(439,278);
				dian6->SetLocation(402,302);
				dian7->SetLocation(471,314);
				dian8->SetLocation(464,361);
				dian9->SetLocation(438,400);
				dian10->SetLocation(338,367);
				dian11->SetLocation(417,161);
				wenzi->SetText("冲脉：");
				this->update(jm3);
			}
			break;
			case 29:
			{
			
				if(select==0)
				{
					return false;
				}
				CRoleItem* pRoleItem = this->GetMaterialItem();
				if(!pRoleItem) 
				{
					return false;
				}
				stFujianJingmaiLevelup cmd;
				cmd.jingm=select; 
			    cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			    SEND_USER_CMD(cmd);
				//jm1++;
				//this->update(jm1);
			}
			break;
		}

		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

CRoleItem* CGuijingmaiDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}

void CGuijingmaiDlg::update(int num)
{

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	jingmaipeizhi sjs;
	for(int i=0;i<num;i++)
	{
		sjs = g_jingmai[i]; 
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}

	char nums[256];
	jingmaipeizhi zt = g_jingmai[num];
	sprintf(nums,"+%d",pDam);//看明白没
	GetStatic(120)->SetText(nums);
	sprintf(nums,"+%d",mDam);
	GetStatic(121)->SetText(nums);
	sprintf(nums,"+%d",pDef);
	GetStatic(122)->SetText(nums);
	sprintf(nums,"+%d",mDef);
	GetStatic(123)->SetText(nums);
	sprintf(nums,"+%d",hp);
	GetStatic(124)->SetText(nums);

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
	if(num<700)
	{
		moneytext->SetText(moneytexts);
		ObjectBase_t* pObject = GetObjectBase(zt.costID);
		char cailiao[256];
		sprintf(cailiao,"           需要%s 数量:%d",pObject->strName,zt.costNum);
		cltext->SetText(cailiao);
	}

	int jie =1;//阶段
	
	if(num<50)
	{
		jie=1;
	}
	else{
		jie= (num/50)+1;
	}

	//等我一会

	//下面算法要重写

	//这里要写经脉阶级
	num-=(jie-1)*50;
	CGuiStatic *jieji = GetStatic(105);
	char jjtext[32];
	sprintf(jjtext,"%d阶-%d级",jie,num);
	jieji->SetText(jjtext);
	

	

	int diannum=0;
	if(num<5){
		diannum = 0;
	}	

	// else if(num%5==0){
	// 	diannum=num/5;
	
	// }
	else{
		diannum = (num/5);
		if(num==50)
		{
			diannum=0;
		}
	}

	

	if(jie==1)
	{
		quanbj2->SetImage(stResourceLocation("data\\interfaces3.gl",1,26));
	}
	else if(jie==2)//所以 或
	{		
		quanbj2->SetImage(stResourceLocation("data\\interfaces3.gl",1,27));
	}
	else if(jie==3)
	{
		quanbj2->SetImage(stResourceLocation("data\\interfaces3.gl",1,28));
	}
	else if(jie==4)
	{
		quanbj2->SetImage(stResourceLocation("data\\interfaces3.gl",1,29));
	}
	else{
		quanbj2->SetImage(stResourceLocation("data\\interfaces3.gl",1,30));
	}
	


	switch (diannum)
	{
	case 0:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			
			if(select==1)
			{
				dian11->SetLocation(411,130);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(378,154);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(417,161);//这个 小黄点 第一个坐标 经脉3
			}
		}
		break;
	case 1:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(410,172);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(330,188);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(437,189);//这个 小黄点 第一个坐标 经脉3
			}//2
		}
		break;
	case 2:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(361,210);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(411,189);//这个 小黄点 第一个坐标 经脉2 
			}
			else{
				dian11->SetLocation(387,202);//这个 小黄点 第一个坐标 经脉3
			}//3
		}
		break;
	case 3:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(468,213);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(366,219);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(422,235);//这个 小黄点 第一个坐标 经脉3
			}//4
		}
		break;
	case 4:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(445,263);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(371,256);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(435,272);//这个 小黄点 第一个坐标 经脉3
			}//5
		}
		break;
	case 5://哪个
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(350,319);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(304,310);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(398,298);//这个 小黄点 第一个坐标 经脉3
			}//6
		}
		break;
	case 6:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(395,375);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(424,289);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(467,310);//这个 小黄点 第一个坐标 经脉3
			}//7
		}
		break;
	case 7:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(478,375);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(377,344);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(460,357);//这个 小黄点 第一个坐标 经脉3
			}//8
		}
		break;
	case 8:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(516,339);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(428,359);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(434,396);//这个 小黄点 第一个坐标 经脉3
			}//9
		}
		break;
	case 9:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,12));
			if(select==1)
			{
				dian11->SetLocation(432,323);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(468,440);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(334,363);//这个 小黄点 第一个坐标 经脉3
			}//10
		}
		break;
	case 10:
		{
			dian1->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian2->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian3->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian4->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian5->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian6->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian7->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian8->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian9->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			dian10->SetImage(stResourceLocation("data\\interfaces3.gl",1,13));
			if(select==1)
			{
				dian11->SetLocation(411,130);//这个 小黄点 第一个坐标 经脉1
			}
			else if(select==2){
				dian11->SetLocation(378,154);//这个 小黄点 第一个坐标 经脉2
			}
			else{
				dian11->SetLocation(413,167);//这个 小黄点 第一个坐标 经脉3
			}//第一个
		}
		break;
	}


}
void CGuijingmaiDlg::SHUAXIN()
{
	if(jm1==0)
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,25));
	}
	else
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,24));
	}
	if(jm2==0)
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,25));
	}
	else
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,24));
	}
	if(jm3==0)
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,25));
	}
	else
	{
		CGuiImage *img = GetImage(14);
		img->SetImage(stResourceLocation("data\\interfaces3.gl",1,24));
	}

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	jingmaipeizhi sjs;

	for(int i=0;i<jm1;i++)
	{
		sjs = g_jingmai[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<jm2;i++)
	{
		sjs = g_jingmai[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<jm3;i++)
	{
		sjs = g_jingmai[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}

	char num[256];
	sprintf(num,"+%d",pDam);
	GetStatic(125)->SetText(num);
	sprintf(num,"+%d",mDam);
	GetStatic(126)->SetText(num);
	sprintf(num,"+%d",pDef);
	GetStatic(127)->SetText(num);
	sprintf(num,"+%d",mDef);
	GetStatic(128)->SetText(num);
	sprintf(num,"+%d",hp);
	GetStatic(129)->SetText(num);


	if(select ==1)
	{
		this->update(jm1);
	}
	else if(select==2)
	{
		this->update(jm2);
	}
	else{
		if(select !=0 ){
			this->update(jm3);
		}
	}
	
}


void CGuijingmaiDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guijingmai = NULL;

	FUNCTION_END;
}