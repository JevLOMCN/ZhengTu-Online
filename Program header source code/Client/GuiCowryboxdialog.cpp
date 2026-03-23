/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-05-14 15:39:16
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-06-27 22:04:41
 * @FilePath: \Client\Client\GuiCowryboxdialog.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
*\file		GuiCowrybox.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2007-11-26  17:20:00 中国标准时间
*\brief	    新加宝盒对话框
*
* 
*
*/


#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiCowryboxdialog.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiMoneyInputBox.h"
#include "./GuiTrade.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiEquip.h"
#include "./GuiUserInfo.h"
#include "../luabind/public.h"
#include "./Game.h"

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiCowrybox::CGuiCowrybox(void)
: m_MyLTime(xtimeGetTime())
, m_Endid(1)
, m_tlbe(1)
, m_sheep(1)
, m_Zzhuans(79)
, m_Jzhuans(0)
, m_TaleTy(false)
, m_TiquTy(false)
, m_AniEnd(true)
, m_aniOk(false)
{
	FUNCTION_BEGIN;
	m_bCloseIsHide = true;
	m_bShowEquip = false;
	//m_clientOffsetTopLeft = stPointI(0,0);
	//m_clientOffsetRightBottom = stPointI(0,0);
	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 创建对话框
 * 
 * \return 无
 */
void CGuiCowrybox::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

//	m_pImage = GetImage(1);
	m_pTable[0] = GetTable(100);
	m_pTable[0]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[0]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE0);
	m_pTable[1] = GetTable(101);
	m_pTable[1]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[1]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE1);
	m_pTable[2] = GetTable(102);
	m_pTable[2]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[2]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE2);
	m_pTable[3] = GetTable(103);
	m_pTable[3]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[3]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE3);
	m_pTable[4] = GetTable(104);
	m_pTable[4]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[4]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE4);
	m_pTable[5] = GetTable(105);
	m_pTable[5]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[5]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE5);
	m_pTable[6] = GetTable(106);
	m_pTable[6]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[6]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE6);
	m_pTable[7] = GetTable(107);
	m_pTable[7]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[7]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE7);
	m_pTable[8] = GetTable(108);
	m_pTable[8]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[8]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE8);
	m_pTable[9] = GetTable(109);
	m_pTable[9]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
    m_pTable[9]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE9);
	m_pTable[10] = GetTable(110);
	m_pTable[10]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[10]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE10);
	m_pTable[11] = GetTable(111);
	m_pTable[11]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[11]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE11);
	m_pTable[12] = GetTable(112);
	m_pTable[12]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[12]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE12);
	m_pTable[13] = GetTable(113);
	m_pTable[13]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[13]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE13);
	m_pTable[14] = GetTable(114);
	m_pTable[14]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[14]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE14);
	m_pTable[15] = GetTable(115);
	m_pTable[15]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[15]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE15);
	m_pTable[16] = GetTable(116);
	m_pTable[16]->m_iTableType = OBJECTCELLTYPE_COWRYBOX;
	m_pTable[16]->m_EquipPosition = stPointI(0,COWRYBOX_TALIE16);

	GetButton(3)->SetVisible(false);

/* 	GetButton(1)->SetVisible(true);
	GetButton(2)->SetVisible(true);
	
	GetButton(4)->SetVisible(true);

//	GetCheckBox(12)->SetVisible(false);
	GetCheckBox(5)->SetVisible(true);
//	GetCheckBox(7)->SetVisible(true);
//	GetButton(8)->SetVisible(true);

//	GetStatic(10)->SetVisible(false);
 */
	//GetImage(0)->SetVisible(false);
	//AddItem(m_pTable[0],501,0);
	//AddItem(m_pTable[1],502,1);
	//AllSetVisible(false);
	
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 151;
	rl.frame = 101;
	POINT pt = GetTable(100)->GetLocation();
	AddImage(300,pt.x-233,pt.y-218,rl);

	//POINT pt = m_pTable[0]->GetLocation();
	//m_aniBegin.Render( pt.x, pt.y );
	//m_aniBegin.Create(&rl);
	//m_aniBegin.SetLoopPlay(true);
	
	stResourceLocation rlani;
	rlani.SetFileName("data\\interfaces.gl");
	rlani.group = 151;
	rlani.frame = 100;
	m_aniBegin.Create( &rlani );
	m_aniBegin.SetLoopPlay(false);
	m_aniBegin.SetSpeed2(30);

	GetButton(2)->SetEnabled(false);
	GetButton(4)->SetEnabled(false);

	FUNCTION_END;
}

void CGuiCowrybox::AllSetVisible(bool visible)
{
	for(int i=0; i<17; i++)
	{
		m_pTable[i]->SetVisible(visible);
	}

	GetButton(1)->SetVisible(visible);
	GetButton(2)->SetVisible(visible);
	GetButton(4)->SetVisible(visible);
	GetCheckBox(5)->SetVisible(visible);
	GetImage(0)->SetVisible(visible);

}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiCowrybox::OnClose(void)
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiCowrybox)
	{
		stSetCowBoxTiquCmd cmd;
		cmd.item_id = !m_TiquTy;
		SEND_USER_CMD( cmd );
		
	    m_arrayCell = NULL;
		//box_tye = 0;
		//Engine_WriteLogF("box close box_tye:%d\n", box_tye);
	    
	    GetGameGuiManager()->m_guiCowrybox = NULL;

		if(GetCheckBox(5)->GetChecked())
		{
			stSetCowBoxKeyCmd cmd;
			cmd.Key_id = m_Keyid;
			//cmd.qwThisID = pSrcItem->GetThisID();
			cmd.isContinue = true;
			SEND_USER_CMD( cmd );
			/* GetGameGuiManager()->AddCowrybox();
			GetGameGuiManager()->m_guiCowrybox->AddItem(pListCmd);
			GetGameGuiManager()->m_guiCowrybox->SetVisible(true);
			GetGameGuiManager()->m_guiCowrybox->SetEnabled(true); */
			//Engine_WriteLogF("继续\n");
			
		}
		Close();
	}


	FUNCTION_END;
}

/**
 * \brief 设置玩家角色
 * 
 * 设置玩家角色
 * 
 * \param type : 角色类型
 * \return 无
 */
void CGuiCowrybox::SetCharType(int type)
{
	FUNCTION_BEGIN;

	if (GetGameGuiManager()->m_guiUserInfo)
		GetGameGuiManager()->m_guiUserInfo->SetCharType(type);

	FUNCTION_END;
}


/**
 * \brief 设置脚本函数
 * 
 * 设置脚本函数
 * 
 * \return 无
 */
void CGuiCowrybox::OnInitScript()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief 对话框消息处理
 * 
 * 对话框消息处理
 * 
 * \param nEvent : 消息类型
 * \param nID : 控件ID
 * \param pControl : 控件指针
 * \return 消息返回值
 */
bool CGuiCowrybox::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 1:
			{
				GetGameGuiManager()->m_guiCowrybox->OnClose();
			}
			return true;

		case 2:
			{
				if(!m_TiquTy)
			    {
				    m_TaleTy = true;
					m_AniEnd = false; //宝盒开始转动并把转动结束标志设为假
					m_TiquTy = true;  //将宝盒是否转动过的标志设为真
				    m_bShowEquip = true;
					m_Jzhuans = 1;
					//m_Zzhuans = 16*5+5;
					m_sheep = 10;
					m_MyLTime = xtimeGetTime();
					GetButton(2)->SetEnabled(false);
					GetButton(4)->SetEnabled(false);
			    }
				else
					Engine_WriteLogF("已经转动过一次！！不可以在转动拉！！\n");
			}
			return true;

	    case 4:
			{
				//发送用户点击 提取按键 的消息 并关闭宝盒控件
				if(!m_AniEnd)    //宝盒转动没有结束不可以提取物品
				{
					Engine_WriteLogF("宝盒转动没有结束,不可以提取物品！");
					return true;
				}
				/* if(!m_TiquTy)    //检测 提取事件 是否是在宝盒转动前触发
				{
					Engine_WriteLogF("发送提取消息1");
				    stSetCowBoxTiquCmd cmd;
					cmd.item_id = true;
	                SEND_USER_CMD( cmd );
				}
				else
				{
					Engine_WriteLogF("发送提取消息2");
					stSetCowBoxTiquCmd cmd;
					cmd.item_id = false;
					SEND_USER_CMD( cmd );
				}  */
				GetGameGuiManager()->m_guiCowrybox->OnClose();
			}
			return true;
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


/**
 * \brief 设置角色名字
 * 
 * 设置角色名字
 * 
 * \param name : 角色名字
 * \return 无
 */
void CGuiCowrybox::SetCharName(const char* name)
{
	FUNCTION_BEGIN;

///	m_pEquip->SetCharName(name);

	FUNCTION_END;
}

/**
 * \brief 刷新对话框
 * 
 * 刷新对话框
 * 
 * \param fElapsedTime : 刷新时间
 * \return 处理结果
 */
HRESULT CGuiCowrybox::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	if(m_aniBegin.IsPlaying())
	{
		stPointI ptani;
		ptani.x = GetDevice()->GetWidth()/2 - m_aniBegin.GetWidth()/2;
		ptani.y = GetDevice()->GetHeight()/2 - m_aniBegin.GetHeight()/2 - 50;
		m_aniBegin.Render( ptani.x, ptani.y );
	}else if(!m_aniOk)
	{
		m_aniOk = true;
		GetButton(2)->SetEnabled(true);
		GetButton(4)->SetEnabled(true);
	}
	
	DingTime();
	
	return hr;
	FUNCTION_END;
}


void CGuiCowrybox::AddItem(stGetCowBoxInitCmd* pCmd)
{
	for(int i=0; i<17; i++)
	{

		CRoleItem* pItem = GetCellItem(m_pTable[i] ,stPointI(0,i));
		if( NULL != pItem)
		{
			pItem->OnRemove();
			delete pItem;
			pItem = NULL;
		}

		t_Object pObj = pCmd->objects[i];
		pObj.pos.dwLocation = OBJECTCELLTYPE_COWRYBOX;
		pObj.pos.y			= i;   //设置显示在宝盒格子的位置
		pObj.pos.x			= 0;

		//Engine_WriteLogF("物品名称:%s - 物品类别ID:%u - 物品位置:%d\n",pCmd->objects[i].strName,pCmd->objects[i].dwObjectID,i);

		pItem = new CRoleItem();
		pItem->SetObject(&pObj);

		//CGuiItemCell* arrayCell = m_pTable[i]->GetItemCell(0,i);
		//arrayCell->GetPos();
		//arrayCell->GetSize();
		/* CGuiItemCell* arrayCell = new CGuiItemCell();
		arrayCell->SetPos(0,i);
		arrayCell->SetSize(1,1);
		arrayCell->m_pItemData = (void*)pItem;
		m_pTable[i]->AddItem(arrayCell); */

	}
	//delete arrayCell;
	//delete pItem;
	m_Zzhuans = 16*5+pCmd->item_Ti;
}


//临时的物品填充宝盒函数
void CGuiCowrybox::AddItem(CGuiTable*  pTable,DWORD id, WORD buffe)
{
	Engine_WriteLogF("物品ID:%d - 物品位置:%d\n",id,buffe);
	ObjectBase_t* pObject = GetObjectBase(id);
	//POINT ptCell = pTable->AppendItemPosition(pObject->cx,pObject->cy);

	t_Object pObj;
	memset(&pObj, 0, sizeof(pObj));
	pObj.dwObjectID = pObject->dwID;
	pObj.qwThisID = INVALID_THISID;
	pObj.pos.dwLocation = OBJECTCELLTYPE_COWRYBOX;
	pObj.pos.dwTableID = 0;
	//pObj.pos.x = ptCell.x;
	pObj.pos.y = buffe;
	pObj.color = -1;
	pObj.dwNum = 1;

	CRoleItem* pItem = new CRoleItem();

	CGuiItemCell* arrayCell = new CGuiItemCell;
	pItem->SetObject(&pObj);
	arrayCell->m_pItemData = (void*)pItem;
	Engine_WriteLogF("物品ID:%d - 物品位置:%d\n",pItem->GetTObject().dwObjectID,pItem->GetObjectBase()->dwItemPic);
	pTable->AddItem(arrayCell);



	//stAddObjectPropertyUserCmd cmd;
	//cmd.byActionType = EQUIPACTION_INIT;
	//memset(&cmd.object,0,sizeof(cmd.object));
	//strncpy(cmd.object.strName,pObject->strName,sizeof(cmd.object.strName));
	//cmd.object.dwObjectID = pObject->dwID;
	//cmd.object.qwThisID = INVALID_THISID;
	//cmd.object.pos.dwLocation = OBJECTCELLTYPE_COWRYBOX;
	//cmd.object.pos.dwTableID = 0;
	//cmd.object.pos.y = buffe;
	//cmd.object.color = -1;
	//cmd.object.dwNum = 1;
	//GetScene()->GetMainCharacter()->AddItem(&cmd);

}

//宝盒转动动画定时器
void CGuiCowrybox::DingTime()
{
	if(!m_AniEnd)
	{
		if(ComparisonTime(m_sheep))
		{
			if(m_Zzhuans > 0)
			{
				PlayUISound2(80002);
				POINT pt = m_pTable[m_Jzhuans]->GetLocation();
				GetImage(300)->SetLocation(pt.x-233,pt.y-218);
				if(m_Jzhuans >15)m_Jzhuans=0;
				++m_Jzhuans;
				--m_Zzhuans;
				if(m_Zzhuans < 15)
					m_sheep = 80;
			}else{
				m_AniEnd = true;
				PlayUISound2(80003);
				GetButton(4)->SetEnabled(true);
			}
			m_MyLTime = xtimeGetTime();
		}
	}
}

//宝盒转动结束动画定时器
void CGuiCowrybox::DingTime1()
{
	Engine_WriteLogF("结束动画定时器");
	if(!ComparisonTime(4))
	{
		return;
	}
	else
	{
		m_pTable[m_Endid-1]->m_AniTye = 1;
		return;
	}
}

//宝盒开启动画定时器
void CGuiCowrybox::DingTime2()
{
	Engine_WriteLogF("开启动画定时器");
	if(!ComparisonTime(5))
	{
		return;
	}
	else
	{
		m_pTable[0]->m_AniTye = 1;
		return;
	}
}

//利用系统时间来检测动画的播放时间(sheep用来控制不同的时间周期)
bool CGuiCowrybox::ComparisonTime(int sheep)
{
	if(xtimeGetTime()-m_MyLTime < sheep)
		return false;
	else
		return true;
}

//动画渲染
