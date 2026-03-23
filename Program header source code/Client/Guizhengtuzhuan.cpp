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
#include "./Guizhengtuzhuan.h"
#include "./RoleItem.h"
//这里我们要载入一下刚刚那个table.h
#include "./table.h"
#include "Maincharacter.h"
#include "./GameScene.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_JIANZHONG 200		// 剑冢
#define ID_BUTTON_JINGMAI 201		// 经脉
#define ID_BUTTON_SHENGXIAO 202		// 生肖
#define ID_BUTTON_SHENGQI 203		// 圣器
#define ID_BUTTON_ZHENHGTUZHUAN 204 // 征途传
#define ID_BUTTON_TISHENG 29		// 提升

#define ID_BUTTON_CLOSE 1 // 关闭
///////////////////////////////////////////////////////////////////////////////
CGuizhengtuzhuanDlg::CGuizhengtuzhuanDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuizhengtuzhuanDlg::~CGuizhengtuzhuanDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CGuizhengtuzhuanDlg::OnCreate() // OnCreate还记得吧 界面创建成功的时候会调用 我们第一次打开征途传的时候 是没有选择图鉴的 对吧 所以
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	select = 0; // 给select一个初始值 0
	z1=0;
	z2=0;
	z3=0;
	z4=0;
	z5=0;
	z6=0;
	z7=0;
	z8=0;
	z9=0;
	z10=0;
	// 首先 Button类型的十个按钮

	// CGuiButton *abc  = GetButton(300); //你可能会疑问 为什么不可以这样去写  为什么要把前面的 单独放到.h里  因为 如果在cpp里创建这个按钮 他会有区域限制
	// 比如 我在这里创建了一个 abc  这个abc 只能在 OnCreate(){   这个括号里面才有效 除了这个括号 就不存在了 比如 }

	zhuan1 = GetButton(300); // 这zhuan1 =XXXX 就是对.h里面的zhuan1 进行赋值  为什么要分开写呢
	zhuan2 = GetButton(301);
	zhuan3 = GetButton(302);
	zhuan4 = GetButton(303);
	zhuan5 = GetButton(304);
	zhuan6 = GetButton(305);
	zhuan7 = GetButton(306);
	zhuan8 = GetButton(307);
	zhuan9 = GetButton(308);
	zhuan10 = GetButton(309);
	// GetButton(300)意思 就是 去GUI里面寻找 id为300的 button 并且保存到zhuan1（我们自己定义的名字 啥都可以）
	// 接下来去找那个文字 id31 static类型
	wenzi = GetStatic(31);
	// 你来写 你感觉应该等于什么 GetXXXX(31); 这个文字 是static 类型对吧 那么获取的时候一定是 去Gui里寻找id为31 并且是static类型的那个控件
	// 好这就两个控件就初始化完毕了 接下来 我们应该当 第一个按钮点击的时候 去让文字变成 当前选择的是：山大王  对吧
	cltext = GetStatic(27);
	moneytext = GetStatic(28);
	m_pTableMaterial1 = this->GetTable(150);
	m_pTableMaterial1->m_iTableType = OBJECTCELLTYPE_FUJIAN;
	m_pTableMaterial1->m_iTableID = 0;
	m_pTableMaterial1->m_EquipPosition = stPointI(0,5);
	//这四行 目前是设置材料table框 定义材料的 目前有点问题  先这样写 后续 我修复后再教你怎么写
	FUNCTION_END;
}

// OnGuiEvent 这个 也是回调 是当 界面上的某个控件 被点击的时候会执行  并且呢 会附带一个参数过来 名字叫nID  也就是当点击某个控件后 就会执行这个OnGuiEvent方法 并且会告诉你点的是id为哪个的控件
bool CGuizhengtuzhuanDlg::OnGuiEvent(UINT nEvent, UINT nID, CGuiControl *pControl)
{
	FUNCTION_BEGIN;

	// abc.setxxxxx //比如我拿到这里来用 它就不存在了 所以 我们要给他写到.h里面 这样 在cpp的任何地方 都可以去调用
	if (nEvent == EVENT_BUTTON_CLICKED)
	{

		// 所以这里我们要判断一下 nid 是不是我们图鉴按钮的id
		//  if(nID==300)
		//  {
		//  	//什么意思 if 如果 如果这个nid 是300的话 执行括号内的内容
		//  	//当我们 点击id 300图鉴1的按钮 之后 我们要干啥 要修改文字的显示内容 直接写
		//  	wenzi->SetText("当前选择的是：山大王");
		//  	//setText 这些都是控件的固定用法 记住就行 后面我会告诉你都有什么 setText的意思就是设置这个id为 31的文字控件显示的文本 完事 写完了编译
		//  	//报错不要慌 写代码的过程中 报错是最经常的事情 这时候 就需要用到 Debug模式了 （调试用）
		//  }
		//  if(nID==301)
		//  {
		//  	//什么意思 if 如果 如果这个nid 是300的话 执行括号内的内容
		//  	//当我们 点击id 300图鉴1的按钮 之后 我们要干啥 要修改文字的显示内容 直接写
		//  	wenzi->SetText("当前选择的是：飞天猪");
		//  	//setText 这些都是控件的固定用法 记住就行 后面我会告诉你都有什么 setText的意思就是设置这个id为 31的文字控件显示的文本 完事 写完了编译
		//  	//报错不要慌 写代码的过程中 报错是最经常的事情 这时候 就需要用到 Debug模式了 （调试用）
		//  }
		//  if(nID==302)
		//  {
		//  	wenzi->SetText("当前选择的是：巨猿");
		//  }
		//  if(nID==303)
		//  {
		//  	wenzi->SetText("当前选择的是：铁爪鬼王");
		//  }
		//  if(nID==304)
		//  {
		//  	wenzi->SetText("当前选择的是：雪人阿布");
		//  }
		//  if(nID==305)
		//  {
		//  	wenzi->SetText("当前选择的是：地域领主");
		//  }
		//  if(nID==306)
		//  {
		//  	wenzi->SetText("当前选择的是：恶魔统领");
		//  }
		//  if(nID==307)
		//  {
		//  	wenzi->SetText("当前选择的是：恶魔尊主");
		//  }
		//  if(nID==308)
		//  {
		//  	wenzi->SetText("当前选择的是：黑山老妖");
		//  }
		//  if(nID==309)
		//  {
		//  	wenzi->SetText("当前选择的是：马上有钱");
		//  }

		// 我现在教你编程的基本语法
		// 昨天你学会了if

		// int a =3; //这里是一个int（整数型的一个变量 名为a 并且呢 我给他了一个值 = 1 ）  这里能看明白吧 这一行 a=2  如果是3那就会执行 else的内容
		// if(a==1) //如果 a==1  那么 执行下面括号里的内容 a==1 不成立跳出
		// {
		// 	//执行
		// }
		// else if(a==2) //这代表第一个if结束后 又接了一个if判断  继续判断  a==2 成立
		// {
		// 	//a==2成立 执行这里的内容
		// }
		// else //else 这个 是和if固定搭配的一个关键字  代表 否则 也就是说当 前面if括号里面的 条件 不成立的时候 就会执行 else里的内容 a是2 那就会执行 else
		// {
		// 	//a不是1的时候执行
		// }

		// 这就是 if判断的结构 用法   当然还可以这么写  明白了吗 if就这三种用法 没了

		// 接下来 switch判断 也是编程里 不管是java还是c 还是c++  都有的一种判断用的  非常常用

		// 这是switch判断的结构 switch后面有个括号 （这里面要把你想进行判断的变量 放进去）
		//  int a =3;
		//  switch (a)//这就代表 我们即将 对a这个变量 进行 switch判断
		//  {
		//  case 1:  //case 就是他的分支  需要怎么用   case 1:   case 2:   case 3:
		//  	{
		//  		//这里 就是当 a=1的时候执行

		// 	}
		// break;
		// case 2:  //case 就是他的分支  需要怎么用   case 1:   case 2:   case 3:
		// 	{

		// 		//这里 就是当 a=2的时候执行
		// 	}
		// break;
		// case 3:  //case 就是他的分支  需要怎么用   case 1:   case 2:   case 3:
		// 	{
		// 		//这里 就是当 a=3的时候执行

		// 	}
		// break;
		// default:
		// 	{
		// 		//这里 就是当前面几个 case都没有成立的时候 来执行  switch的用法就是这么简单 就这一件事 swich（）括号里的变量 用case去判断
		// 	}
		// 	break;
		// }

		// 所以说这里 我们用 switch来写

		//我们接下来要做的就是 当材料放到了框里并且 点击提升按钮的时候 去 提交到服务端上

		switch (nID)
		{
		case ID_BUTTON_CLOSE: // 关闭
		{
			SetVisible(false);
		}
		break;
		case ID_BUTTON_JINGMAI: // 经脉
		{
			GetGameGuiManager()->Addjingmai();
			SetVisible(false);
		}
		break;
		case ID_BUTTON_JIANZHONG: // 剑冢
		{
			GetGameGuiManager()->Addjianzhong();
			SetVisible(false);
		}
		break;
		case ID_BUTTON_SHENGXIAO: // 生肖
		{
			GetGameGuiManager()->Addshengxiao();
			SetVisible(false);
		}
		break;
		case ID_BUTTON_SHENGQI: // 圣器
		{
			GetGameGuiManager()->Addshengqi();
			SetVisible(false);
		}
		break;
		case ID_BUTTON_ZHENHGTUZHUAN: // 征途转
		{
		}
		break;
			// 这里明白了吗
			// 然后来到这里 当我们选择了某个征途传的时候 直接给select赋值
			// 这样就写好了 也就是 我点击某一个图鉴 select的值都会随之变化 懂了吗 这里 好 接下来 就是我们要点击提升按钮 先去GUI 看一下 提升按钮的ID 是啥玩意 29 直接switch里添加一个case
		case 300:
		{
			//if直接在case里面写就可以了
			char a[32]; //这里就是这么写
			//sprintf(a,"征途传-山大王 当前等级：%d",z1);
			//wenzi->SetText(a);
			select = 1;
			this->update(z1);
				if(z1==0)
				{
					wenzi->SetText("征途传-山大王尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-山大王 当前等级：%d",z1);
			        wenzi->SetText(a); 				
				}
		}
		break;
		case 301:
		{
			char a[32];
			select = 2;
			this->update(z2);
				if(z2==0)
				{
					wenzi->SetText("征途传-飞天猪尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-飞天猪 当前等级：%d",z2);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 302:
		{
			char a[32];
			select = 3;
			this->update(z3);
				if(z3==0)
				{
					wenzi->SetText("征途传-巨猿尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-巨猿 当前等级：%d",z3);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 303:
		{
			char a[32];
			select = 4;
			this->update(z4);
				if(z4==0)
				{
					wenzi->SetText("征途传-铁爪鬼王尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-铁爪鬼王 当前等级：%d",z4);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 304:
		{
			char a[32];
			select = 5;
			this->update(z5);
				if(z5==0)
				{
					wenzi->SetText("征途传-雪人阿布尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-雪人阿布 当前等级：%d",z5);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 305:
		{
			char a[32];
			select = 6;
			this->update(z6);
				if(z6==0)
				{
					wenzi->SetText("征途传-地域领主尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-地域领主 当前等级：%d",z6);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 306:
		{
			char a[32];
			select = 7;
			this->update(z7);
				if(z7==0)
				{
					wenzi->SetText("征途传-恶魔统领尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-恶魔统领 当前等级：%d",z7);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 307:
		{
			char a[32];
			select = 8;
			this->update(z8);
				if(z8==0)
				{
					wenzi->SetText("征途传-恶魔尊主尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-恶魔尊主 当前等级：%d",z8);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 308:
		{
			char a[32];
			select = 9;
			//当我们切换的时候 也去刷新一下 等级
			this->update(z9);
				if(z9==0)
				{
					wenzi->SetText("征途传-黑山老妖尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-黑山老妖 当前等级：%d",z9);
			        wenzi->SetText(a); 					
				}
		}
		break;
		case 309:
		{
			char a[32];
			select = 10;
			this->update(z10);
				if(z10==0)
				{
					wenzi->SetText("征途传-马上有钱尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-马上有钱 当前等级：%d",z10);
			        wenzi->SetText(a);  					
				}
		}
		break;

		case 29: // 这个就是 提升按钮点击后 执行的内容 这里没问题吧    //这个要保留 case 29 是判断点击的按钮是否是 提升按钮用的
		{
			
				//这里是提交
			if(GetScene()->GetMainCharacter()->GetLevel()<210)
			{
				cltext->SetText("210级开放征途转功能！");
				return false;
			}

			if(select==0)//0就是没选择 能看懂吧
			{
				return false;
			}
			//接下来 我们 点击提升按钮的时候

			//接下来 我们要在点击提升按钮的时候 去 请求
			CRoleItem* pRoleItem = this->GetMaterialItem(); //获取 格子内的道具
			if(!pRoleItem) //如果没有道具 返回
			{
				return false;
			}
			//请求提升征途转
			stFujianZtzLevelup cmd; //sky 请求指令
			cmd.zhuan=select; 
			cmd.itemUID = pRoleItem->GetObject()->qwThisID;
			SEND_USER_CMD(cmd);

			
		}
		break;
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent, nID, pControl);
	FUNCTION_END;
}


CRoleItem* CGuizhengtuzhuanDlg::GetMaterialItem()
{
	CRoleItem* pRoleItem = NULL;
	CGuiItemCell* pItemCell = m_pTableMaterial1->GetItemCell(0, 0);
	if (pItemCell)
	{
		pRoleItem = static_cast<CRoleItem*>(pItemCell->m_pItemData);
	}

	return pRoleItem;
}


//稍等一会 我把昨天的问题弄好 马上
//我们在方法里（这里是指这个方法在调用时需要传递的参数 这我们先写一个int类型的参数叫 level） 这里不能叫level因为 变量名字冲突了
void CGuizhengtuzhuanDlg::update(int num) // 这里 我们定义了一个方法 叫update 后续用来 刷新界面用 我们在cpp里创建了这个方法 那么 .h里也要声明一下

//这里的num你可以随便起名 他就是一个int类型的整数 并且可以在 下面的代码区域 使用

//在你写代码的时候 你可以理解 这是个临时的变量 他现在是没有值的 只不过我们在上面调用的时候 去给他赋值了 因为这样 我们就不用写一大堆 更改等级的代码了 我们只需要写这一个方法 然后 再点击不同图鉴的时候  传递进来不同的等级即可 明白了吧 
{
	// 当我们的 等级变化了 那这里的当前等级 也得跟着变化 去 gui 找  51 当前等级   54 下一级
	CGuiStatic *level = GetStatic(51); // 这里 是找到了 leve这个 static 类型的 控件 和那个当前选择是：山大王 是一样的
	// 然后 这里就是新的基础数据类型 int代表整数型   而char 代表字符 abcdefg...一切非数字的内容 都可以用 char 来声明变量 来存储
	// char a[32]; 这是声明char类型变量的固定写法 32 代表 这个变量的最大字符长度 不能超越 否则会报错  所以说 你看 我们需要存的是什么 X级 对不对 就这两个字
	// 所以32绝对够用 当然 你也可以随便写 写大一点 也没关系
	char a[256];
	// 声明完之后 记不记得int类型 比如 int a；  a=1; 可以直接给a 赋值对吧  把1 赋值给a  int类型是可以用等号=来直接赋值的 但是char不可以
	// 在C语言中不可以 别的语言比如java是可以的  char a = "A"; 这样是可以的在别的语言
	// 在这里 我们需要用到 sprintf()方法  这个是c++自带的一个方法
	// 括号里需要填写2个参数
	// sprntf(待操作变量，内容)
	// sprintf(a,"我爱你"); //这里 我们就是 把 我爱你 这个字符串 赋值给了 a  你可以理解sprintf（）就是赋值 只不过char类型很特殊 不能直接用=等于 懂了没

	// 上面这种 是直接将一个固定的字符串 我爱你 赋值给了 a  但是 我们要做的 是一个变量 +级 这样的字符串

	//比如这 我刚跟你说得赋值
	sprintf(a, "%d级", num); // 也就是 把 z1的值放到了 %d的位置上 这个是不是不太好理解 嗯 然后 这个,后面的内容 一定要按顺序来 比如
	// 比如这里有三个占位符 那你后面一定要跟三个变量 而且 是一一对应的 z1 对应第一个%d z2 对应第二个 明白了吧 这里我们只需要一个
	// 也就是 这个  x级 这个x取的是 变量 z1得值 所以我们要在 第二个参数内 "%d级" %d你可以理解为一个占位符号 %d是一个整数型的占位符 他的值 是根据后面,后的内容赋值
	level->SetText(a);
	CGuiStatic *level2 = GetStatic(54); // 这里是第二个控件 也就是 下一级那个控件
	// 还是一样 赋值 但后面是z1+1  因为 z1 是我们当前的等级， 所以赋值的时候 要给他 +1 然后赋值进去 因为是下一级嘛
	sprintf(a, "%d级", num + 1);
	level2->SetText(a);


	// 但是呢 我们这里写的这些  给等级控件赋值的地方 下面有那么多case 我们要挨个写一个 很费劲 所以 我们给他拿出来

	//拿到我们刚刚加的方法里面

	//这里呢 我们后续还要加别的内容 就是下面的属性 当前等级属性 和 下一级等级属性 


	//我这边录屏不行 你客户端我没有 我直接远程教你 把征途转写完 完了我去睡觉  
	//接下来我们要实现的功能就是 属性加成  
	//属性加成这里 我们要取 xml配置文件 我们先想想 取xml配置文件 该怎么写
	//首先 这个xml配置文件 我们需要 在客户端读取 并且 服务端也要读取 客户端读取是为了 显示 当前等级的属性 

	//回到这里 我们已经获取到了 征途转的等级对吧
	//num 我们根据num的等级 读取刚刚存好的100信息 显示到 页面上
	//总属性 这里是总属性


	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	//上面新建了5个变量 用来存 我们五个属性


	ztzpeizhi sjs;
	for(int i=0;i<num;i++)
	{
		sjs = g_ztz[i]; //这里 就运用到了i 我们取 g_ztz 的第i个 i是从0开始循环一直自增到z1 的 所以 比如 z1是50级 那么 循环50次 把每一次的下面的五个属性 都加到我们上面创建的五个变量里
		//明白了吗 这是累加 你要是不太明白你就说
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}

	char nums[256];
	sprintf(nums,"+%d",pDam);
	GetStatic(111)->SetText(nums);
	sprintf(nums,"+%d",mDam);
	GetStatic(112)->SetText(nums);
	sprintf(nums,"+%d",pDef);
	GetStatic(113)->SetText(nums);
	sprintf(nums,"+%d",mDef);
	GetStatic(114)->SetText(nums);
	sprintf(nums,"+%d",hp);
	GetStatic(115)->SetText(nums);

	//下一级
	ztzpeizhi zt = g_ztz[num];
	sprintf(nums,"+%d",zt.pDam);
	GetStatic(121)->SetText(nums);
	sprintf(nums,"+%d",zt.mDam);
	GetStatic(122)->SetText(nums);
	sprintf(nums,"+%d",zt.pDef);
	GetStatic(123)->SetText(nums);
	sprintf(nums,"+%d",zt.mDef);
	GetStatic(124)->SetText(nums);
	sprintf(nums,"+%d",zt.hp);
	GetStatic(125)->SetText(nums);

	//懂了吗
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

void CGuizhengtuzhuanDlg::SHUAXIN() //创建一个方法叫 刷新
{
	//这里呢就把图鉴按照等级刷新 从z1 刷到 z10
	if(z1>0)
	{
		zhuan1->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 1), 1);
		if(z1>20 && z1<=40) //如果是&& 那么 必须两种条件 同时成立 才会执行里面的内容 这叫 与    如果是|| 或 那么 两种条件 有一个成立则执行   懂了吗
		{
			CGuiImage *img = GetImage(400);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z1>40 && z1<=60)
		{
			CGuiImage *img = GetImage(410);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z1>60 && z1<=80)
		{
			CGuiImage *img = GetImage(420);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z1>80)
		{
			CGuiImage *img = GetImage(430);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
	}
	else
	{
		zhuan1->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 3), 1);
	}

	if(z2>0)
	{
		zhuan2->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 4), 1);
		if(z2>20 && z2<=40)
		{
			CGuiImage *img = GetImage(401);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z2>40 && z2<=60)
		{
			CGuiImage *img = GetImage(411);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z2>60 && z2<=80)
		{
			CGuiImage *img = GetImage(421);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z2>80)
		{
			CGuiImage *img = GetImage(431);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
		
	}
	else
	{
		zhuan2->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 6), 1);
	}
	if(z3>0)
	{
		zhuan3->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 7), 1);
		if(z3>20 && z3<=40)
		{
			CGuiImage *img = GetImage(402);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z3>40 && z3<=60)
		{
			CGuiImage *img = GetImage(412);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z3>60 && z3<=80)
		{
			CGuiImage *img = GetImage(422);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z3>80)
		{
			CGuiImage *img = GetImage(432);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
		
	}
	else
	{
		zhuan3->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 9), 1);
	}

	if(z4>0)
	{
		zhuan4->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 10), 1);
		if(z4>20 && z4<=40)
		{
			CGuiImage *img = GetImage(403);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z4>40 && z4<=60)
		{
			CGuiImage *img = GetImage(413);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z4>60 && z4<=80)
		{
			CGuiImage *img = GetImage(423);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z4>80)
		{
			CGuiImage *img = GetImage(433);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
		
	}
	else
	{
		zhuan4->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 12), 1);
	}

	if(z5>0)
	{
		zhuan5->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 13), 1);
		if(z5>20 && z5<=40)
		{
			CGuiImage *img = GetImage(404);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z5>40 && z5<=60)
		{
			CGuiImage *img = GetImage(414);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z5>60 && z5<=80)
		{
			CGuiImage *img = GetImage(424);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z5>80)
		{
			CGuiImage *img = GetImage(434);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
		
	}
	else
	{
		zhuan5->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 15), 1);
	}

	if(z6>0)
	{
		zhuan6->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 16), 1);
		if(z6>20 && z6<=40)
		{
			CGuiImage *img = GetImage(405);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z6>40 && z6<=60)
		{
			CGuiImage *img = GetImage(415);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z6>60 && z6<=80)
		{
			CGuiImage *img = GetImage(425);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z6>80)
		{
			CGuiImage *img = GetImage(435);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
		
	}
	else
	{
		zhuan6->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 18), 1);
	}

	if(z7>0)
	{
		zhuan7->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 19), 1);
		if(z7>20 && z7<=40)
		{
			CGuiImage *img = GetImage(406);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z7>40 && z7<=60)
				{
			CGuiImage *img = GetImage(416);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z7>60 && z7<=80)
		{
			CGuiImage *img = GetImage(426);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z7>80)
		{
			CGuiImage *img = GetImage(436);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
  	    }
		
	}
	else
	{
		zhuan7->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 21), 1);
	}

	if(z8>0)
	{
		zhuan8->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 22), 1);
		if(z8>20 && z8<=40)
		{
			CGuiImage *img = GetImage(407);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z8>40 && z8<=60)
		{
			CGuiImage *img = GetImage(417);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z8>60 && z8<=80)
		{
			CGuiImage *img = GetImage(427);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z8>80)
		{
			CGuiImage *img = GetImage(437);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
	}
	else
	{
		zhuan8->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 24), 1);
	}

	if(z9>0)
	{
		zhuan9->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 25), 1);
		if(z9>20 && z9<=40)
		{
			CGuiImage *img = GetImage(408);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z9>40 && z9<=60)
		{
			CGuiImage *img = GetImage(418);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z9>60 && z9<=80)
		{
			CGuiImage *img = GetImage(428);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z9>80)
		{
			CGuiImage *img = GetImage(438);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}
	}
	else
	{
		zhuan9->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 27), 1);
	}
	if(z10>0)
	{
		zhuan10->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 28), 1);
		if(z10>20 && z10<=40)
		{
			CGuiImage *img = GetImage(409);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,89));
		}
		if(z10>40 && z10<=60)
		{
			CGuiImage *img = GetImage(419);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,226));
		}
		if(z10>60 && z10<=80)
		{
			CGuiImage *img = GetImage(429);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,227));
		}
		if(z10>80)
		{
			CGuiImage *img = GetImage(439);
			img->SetImage(stResourceLocation("data\\interfaces3.gl",6,228));
		}		
	}
	else
	{
		zhuan10->SetBitmap(stResourceLocation("data\\interfaces3.gl", 6, 30), 1);
	}

	int pDam=0;
	int mDam=0;
	int pDef=0;
	int mDef=0;
	int hp=0;

	//上面新建了5个变量 用来存 我们五个属性


	//然后 创建一个ztzpeizhi

	ztzpeizhi sjs;

	//可以看到 for 来了 这就是编程里最常用的循环 for循环


	// for(int i=0;i<=100;i++)
	// {
		
	// }
	//这是最基本的for循环写法

	//for 后面的括号 有三个 参数 分别用 ;分号分割   第一个参数 是创建一个 整数型的 变量 我们一般都用i 命名为i

	//这个i 必须给一个初始值 这里给了0

	//第二个参数 就是条件 这里和 while一样  是当 这个条件 成立的时候 会一直执行 直到不成立 才会跳出循环   

	//第三个参数 i++  这里代表 每循环一次 这个变量i 会怎么样 
	
	//明白了吗

	//为什么说 for循环是最常用的呢 因为 这个i 可以在for循环的循环体里面 调用  看下面代码 你就明白了
	














	//一样 定义一个i =0  条件是 i<z1 z1是啥？ z1是 征途转山大王 的等级对吧  也就是山大王多少级 就循环 多少次
	for(int i=0;i<z1;i++)
	{
		sjs = g_ztz[i]; //这里 就运用到了i 我们取 g_ztz 的第i个 i是从0开始循环一直自增到z1 的 所以 比如 z1是50级 那么 循环50次 把每一次的下面的五个属性 都加到我们上面创建的五个变量里
		//明白了吗 这是累加 你要是不太明白你就说
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}




	for(int i=0;i<z2;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z3;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z4;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z5;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z6;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z7;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z8;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z9;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	for(int i=0;i<z10;i++)
	{
		sjs = g_ztz[i];
		pDam+=sjs.pDam;
		mDam+=sjs.mDam;
		pDef+=sjs.pDef;
		mDef+=sjs.mDef;
		hp+=sjs.hp;
	}
	char num[256];
	
	sprintf(num,"+%d",pDam);
	GetStatic(131)->SetText(num);
	sprintf(num,"+%d",mDam);
	GetStatic(132)->SetText(num);
	sprintf(num,"+%d",pDef);
	GetStatic(133)->SetText(num);
	sprintf(num,"+%d",mDef);
	GetStatic(134)->SetText(num);
	sprintf(num,"+%d",hp);
	GetStatic(135)->SetText(num);	



	char a[32]; //这里就是这么写

	switch(select)
	{
		case 1:
			{
				this->update(z1);
				if(z1==0)
				{
					wenzi->SetText("征途传-山大王尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-山大王 当前等级：%d",z1);
			        wenzi->SetText(a); 				
				}
			}
			break;
		case 2:
			{
				this->update(z2);
				if(z2==0)
				{
					wenzi->SetText("征途传-飞天猪尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-飞天猪 当前等级：%d",z2);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 3:
			{
				this->update(z3);
					if(z3==0)
				{
					wenzi->SetText("征途传-巨猿尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-巨猿 当前等级：%d",z3);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 4:
			{
				this->update(z4);
				if(z4==0)
				{
					wenzi->SetText("征途传-铁爪鬼王尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-铁爪鬼王 当前等级：%d",z4);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 5:
			{
				this->update(z5);
				if(z5==0)
				{
					wenzi->SetText("征途传-雪人阿布尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-雪人阿布 当前等级：%d",z5);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 6:
			{
				this->update(z6);
				if(z6==0)
				{
					wenzi->SetText("征途传-地域领主尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-地域领主 当前等级：%d",z6);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 7:
			{
				this->update(z7);
				if(z7==0)
				{
					wenzi->SetText("征途传-恶魔统领尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-恶魔统领 当前等级：%d",z7);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 8:
			{
				this->update(z8);
				if(z8==0)
				{
					wenzi->SetText("征途传-恶魔尊主尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-恶魔尊主 当前等级：%d",z8);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 9:
			{
				this->update(z9);
				if(z9==0)
				{
					wenzi->SetText("征途传-黑山老妖尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-黑山老妖 当前等级：%d",z9);
			        wenzi->SetText(a); 					
				}
				
			}
			break;
		case 10:
			{
				this->update(z10);
				if(z10==0)
				{
					wenzi->SetText("征途传-马上有钱尚未激活");
				}
				else
				{
			        sprintf(a,"征途传-马上有钱 当前等级：%d",z10);
			        wenzi->SetText(a);  					
				}
			}
			break;


	}
}
void CGuizhengtuzhuanDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guizhengtuzhuan = NULL;

	FUNCTION_END;
}