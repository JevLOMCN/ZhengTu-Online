/**
*\file		GuiWorldConn.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2005-7-5   17:12:00 中国标准时间
*\brief	    社会关系界面
*
* 
*
*/


#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./MainCharacter.h"
#include "./GameScene.h"
#include "./GuiPrivateChat.h"
#include "./GuiTeam.h"
#include "./GuiMessageBox.h"
#include "./GuiBallot.h"
#include ".\guiworldconn.h"
#include "./GuiTrade.h"
#include "./CharacterProperty.h"
#include "./GuiUnoinPower.h"
#include "./GuiSendMailDialog.h"
#include "./Country.h"
#include "./CountryInfo.h"
#include "./GuiAdoptCartoonDialog.h"
#include "./GuiNpcDialog.h"
#include "./GuiApplyConn.h"
#include "./GuiShopDialog.h"
#include "./GuiChatOutput.h"
#include "./GuiPetDialog.h"
#include "./Chat.h"
#include "./Relation.h"

#include "./GuiMasterBountyDlg.h"
#include "./GuiPrenticeBountyDlg.h"

#include "./GameCursor.h"

#ifdef _DEBUG
#include "../engine/include/Spell.h"
#endif

#include "./GuiTop.h"
#include "./GuiTaiMiaoTop.h"
#include "./GuiHalofFameTop.h"

#define WorldConnDialog_SetButtonEnable(id,b) {CGuiButton* p = GetButton(id);if(p) {p->SetEnabled(b);}}

///对话框左上角位置
const stPointI  clientOffsetTopLeft(15,15);

///对话框右下角位置
const stPointI  clientOffsetRightBottom(15,15);

///当前选中的位置
static char     s_szCurSel[MAX_NAMESIZE]={0};

///附近玩家的最大数量
const  int      maxNearNum = 100;

///社会关系的称号
const stWorldConnGrade g_WorldConnGrade[]=
{
	{RELATION_TYPE_LOVE,    "夫妻","一见钟情","百年好合","永结同心","神仙眷侣"},
	{RELATION_TYPE_TEACHER, "师徒","★","★★","★★★","★★★★"},
	{RELATION_TYPE_FRIEND,  "好友","点头之交","君子之交","莫逆之交","生死之交"},
};

///帮会，家族，门派，好友公告板上的提示信息
const stBoardShowInfo g_BoardShowInfo[]=
{
	{eAction_TongBoard,false,CHAT_TYPE_UNION_AFFICHE,"帮会公告板","这里是帮主写下的告示"},
	{eAction_TongBoard,true,CHAT_TYPE_UNION_AFFICHE,"帮会公告板","这里是帮主输入告示的地方"},
	{eAction_SchoolBoard,false,CHAT_TYPE_OVERMAN_AFFICHE,"门派公告板","这里是师尊写下的告示"},
	{eAction_SchoolBoard,true,CHAT_TYPE_OVERMAN_AFFICHE,"门派公告板","这里是师尊输入告示的地方"},
	{eAction_SeptBoard,false,CHAT_TYPE_FAMILY_AFFICHE,"家族公告板","这里是族长写下的告示"},
	{eAction_SeptBoard,true,CHAT_TYPE_FAMILY_AFFICHE,"家族公告板","这里是族长输入告示的地方"},
	{eAction_FriendBoard,false,CHAT_TYPE_FRIEND_AFFICHE,"好友留言板","这里是所有好友给你留言的地方"},
	{eAction_FriendBoard,true,CHAT_TYPE_FRIEND_AFFICHE,"好友留言板","这里是给所有好友留言的地方"},
};

///社会关系菜单
const stPopMenu  g_PopMenu[]=
{
	{FRIEND,FriendButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{FRIEND,FriendButton5,"割席断交",CGuiMenu::MenuStyle_Command},
	{FRIEND,0,"",CGuiMenu::MenuStyle_Separate},
	{FRIEND,FriendButton8,"留言",CGuiMenu::MenuStyle_Command},
	{FRIEND,-1,"",CGuiMenu::MenuStyle_Separate},
	{FRIEND,FriendButton11,"普通组队",CGuiMenu::MenuStyle_Command},
	{FRIEND,FriendButton12,"荣誉组队",CGuiMenu::MenuStyle_Command},	
#ifdef _DEBUG_GROUP
	{FRIEND,FriendButton13,"特殊组队",CGuiMenu::MenuStyle_Command},	
#endif 
	{FRIEND,-1,"",CGuiMenu::MenuStyle_Separate},
	{FRIEND,FriendButton14,"招收徒弟",CGuiMenu::MenuStyle_Command},	
	{FRIEND,FriendButton15,"复制人名",CGuiMenu::MenuStyle_Command},
	
	{SEPT,SeptButton100,"密聊",CGuiMenu::MenuStyle_Command},
	
  //{SEPT,SeptButton3,"开除族人",CGuiMenu::MenuStyle_Command},
  //{SEPT,SeptButton4,"招收族人",CGuiMenu::MenuStyle_Command},
	{SEPT,0,"",CGuiMenu::MenuStyle_Separate},
	{SEPT,SeptButton6,"加为好友",CGuiMenu::MenuStyle_Command},
	{SEPT,SeptButton7,"普通组队",CGuiMenu::MenuStyle_Command},
	{SEPT,SeptButton8,"荣誉组队",CGuiMenu::MenuStyle_Command},
#ifdef _DEBUG_GROUP
	{SEPT,SeptButton9,"特殊组队",CGuiMenu::MenuStyle_Command},
#endif
	{SEPT,SeptButton10,"招收徒弟",CGuiMenu::MenuStyle_Command},	
	{SEPT,SeptButton11,"复制人名",CGuiMenu::MenuStyle_Command},
	
	{TONG,TongButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{TONG,TongButton13,"复制人名",CGuiMenu::MenuStyle_Command},	
	{TONG,TongButton3,"赋予称号",CGuiMenu::MenuStyle_Command},
  //{TONG,TongButton4,"赋予权利",CGuiMenu::MenuStyle_Command},
  //{TONG,TongButton5,"开除成员",CGuiMenu::MenuStyle_Command},
  //{TONG,TongButton6,"招收成员",CGuiMenu::MenuStyle_Command},	
	{TONG,0,"",CGuiMenu::MenuStyle_Separate},
	{TONG,TongButton8,"加为好友",CGuiMenu::MenuStyle_Command},
	{TONG,TongButton9,"普通组队",CGuiMenu::MenuStyle_Command},
	{TONG,TongButton10,"荣誉组队",CGuiMenu::MenuStyle_Command},
#ifdef _DEBUG_GROUP
	{TONG,TongButton11,"特殊组队",CGuiMenu::MenuStyle_Command},
#endif 

	{TONG,TongButton12,"招收徒弟",CGuiMenu::MenuStyle_Command},	

	
	{SCHOOL,SchoolButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{SCHOOL,SchoolButton6,"复制人名",CGuiMenu::MenuStyle_Command},
	
	{NEARBY,NearbyButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton1,"加为好友",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton2,"邀请加入队伍",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton3,"邀请加入家族",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton4,"邀请加入帮会",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton5,"招收徒弟",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton7,"加入黑名单",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton8,"交易",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton9,"查看摊位",CGuiMenu::MenuStyle_Command},
	{NEARBY,NearbyButton10,"复制人名",CGuiMenu::MenuStyle_Command},	
	
	{NEARBYNPC,NearbyNpcButton1,"移动",CGuiMenu::MenuStyle_Command},
	
	{BADUSER,BadUserButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{BADUSER,BadUserButton1,"加入黑名单",CGuiMenu::MenuStyle_Command},
	{BADUSER,BadUserButton2,"解除黑名单",CGuiMenu::MenuStyle_Command},
	{BADUSER,BadUserButton9,"复制人名",CGuiMenu::MenuStyle_Command},
	
	{ENEMYUSER,ENEMYButton100,"密聊",CGuiMenu::MenuStyle_Command},
	{ENEMYUSER,ENEMYButton101,"复制人名",CGuiMenu::MenuStyle_Command},
	{ENEMYUSER,ENEMYButton1,"加入黑名单",CGuiMenu::MenuStyle_Command},
	{ENEMYUSER,ENEMYButton2,"移除",CGuiMenu::MenuStyle_Command},
};


const stRolePic g_RolePicList[]={
	{eOnLine,41},
	{eOffLine,43},
	{eOnlineClew,42},
	{eOffLineClew,44},
};

const char SEPT_BOARD_INFO[] = 
{"                        家族介绍\n一、家族的成立：\n  当你达到35级时，带上20两银子，去民政官处可申请成立家族。\n二、家族的优势：\n1、家族成员打怪练级其它家族成员可获得相应经验。\n2、家族成员之间组队练级还能获得更多经验加成。\n3、占领城市NPC，每日收取NPC管理费用。\n4、可以接到各种经验奖励丰厚的家族任务。\n5、当家族战争（争夺 NPC ）成功后，本家族成员在争夺的 NPC 附近打坐，可获得当前人物打怪效率的 5%-10% 的经验值。\n6、家族成员可以在族长的带领下，领取丰厚的经验。（成员越多，领取的经验越多）\n"};

const char TONG_BOARD_INFO[] =
{"					      帮会介绍\n一、帮会的成立：\n  当你达到45级时，带上天羽令和5锭银子，去大臣处可申请成立帮会。\n二、帮会的优势：\n1、实力强的帮会就可以占领城池，每日收取管理费用。\n2、可以接到各种经验奖励丰厚的帮会任务。\n"};

/**
 * \brief 往map容器插入一条记录
 * 
 * 往map容器插入一条记录
 * 
 * \param p : MAP中元素的数据
 * \param list : MAP容器的引用
 * \param key : MAP中元素的关键字
 * \return 无
 */
template <class ST,class MAPLIST,class KEY>
void InsertListItem(ST* p, MAPLIST& list, KEY key)
{
	FUNCTION_BEGIN;

	list.insert(MAPLIST::value_type(key, *p));

	FUNCTION_END;
}

int GetMaxSeptNumByTongMana(const int& dwMana)
{
	if ( dwMana >= 80000 )
		return 60;//50

	if ( dwMana >= 30000 )
		return 50;//40

	if ( dwMana >= 10000 )
		return 40;//30

	return 30;//20
}

/**
 * \brief map容器删除一条记录
 * 
 * map容器删除一条记录
 * 
 * \param list : MAP容器的引用
 * \param key : MAP中元素的关键字
 * \return 无
 */
template <class MAPLIST,class KEY>
void DeleteListItem(MAPLIST& list,KEY key)
{
	FUNCTION_BEGIN;

	MAPLIST::iterator pointer=list.find(key);
	if (pointer==list.end())
		return ;

	list.erase(key);

	FUNCTION_END;
}

/**
 * \brief  查找map容器中的数据
 * 
 * 查找map容器中的数据
 * 
 * \param list : MAP容器的引用
 * \param key : MAP中元素的关键字
 * \return 查找到MAP中元素的数据
 */
template <class MAPLIST,class KEY>
void* GetListItem(MAPLIST& list,KEY key)
{
	FUNCTION_BEGIN;

	MAPLIST::iterator pointer=list.find(key);
	if (pointer==list.end())
		return NULL;

	return (void*)&(pointer->second);

	FUNCTION_END;
}


/**
 * \brief 得到角色的头像资源
 * 
 * 得到角色的头像资源
 * 
 * \param role : 角色值
 * \param state : 角色头像的类型（在线，不在线，有消息）
 * \return 得到角色的头像资源
 */
stPointI GetResource(byte role,enumRoleState state)
{
	FUNCTION_BEGIN;

	static int size = sizeof(g_RolePicList)/sizeof(stRolePic);
	stPointI  pt(0,0);
	for (int i=0;i<size;i++)
	{
		if (g_RolePicList[i].state==state)
		{
			pt.x=g_RolePicList[i].group;
			pt.y=role;
			break;
		}
	}
	return pt;

	FUNCTION_END;
}

/**
* \brief 加载MENU的内容
* 
* 加载MENU的内容
* 
* \param type : MENU的类别
* \param pMenu : MENU类的指针
* \return 无
*/
void AddMenuItem(BYTE type,CGuiMenu* pMenu)
{
	FUNCTION_BEGIN;

	static int size = sizeof(g_PopMenu)/sizeof(stPopMenu);
	if (!pMenu)
		return;

	for (int i=0;i<size;i++)
	{
		if (type==g_PopMenu[i].menuType)
		{
			pMenu->AddMenuItem(g_PopMenu[i].menuId,
				g_PopMenu[i].menuName,(CGuiMenu::enumMenuStyle)g_PopMenu[i].menuStyle);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 设置MENU的项是否可用
 * 
 * 设置MENU的项是否可用
 * 
 * \param pMenu : MENU类的指针
 * \param cmd : MENU的菜单项
 * \param bEnabled : 是否可用
 * \return 无
 */
void SetMenuEnabled(CGuiMenu* pMenu,int cmd,bool bEnabled)
{
	FUNCTION_BEGIN;

	if (!pMenu)
		return;

	pMenu->SetMenuEnabled(pMenu->GetMenuIndex(cmd),bEnabled);

	FUNCTION_END;
}

/**
 * \brief 设置MENU的所有项不可用
 * 
 * 设置MENU的所有项不可用
 * 
 * \param type : MENU的类别
 * \param pMenu : MENU类的指针
 * \return 无
 */
void SetMenuDisenable(BYTE type,CGuiMenu* pMenu)
{
	FUNCTION_BEGIN;

	static int size = sizeof(g_PopMenu)/sizeof(stPopMenu);
	if (!pMenu)
		return;

	for (int i=0;i<size;i++)
	{
		if (type==g_PopMenu[i].menuType)
		{
			SetMenuEnabled(pMenu,g_PopMenu[i].menuId,false);
		}
	}

	FUNCTION_END;
}

/**
 * \brief 得到社会关系的称号结构
 * 
 * 得到社会关系的称号结构
 * 
 * \param id : 社会关系的称号类别
 * \return 社会关系的称号结构
 */
const stWorldConnGrade* getWorldConn(byte id)
{
	FUNCTION_BEGIN;

	static int n=sizeof(g_WorldConnGrade)/sizeof(stWorldConnGrade);
	for (int i=0;i<n;i++)
	{
		if (id==g_WorldConnGrade[i].conn)
		{
			return &(g_WorldConnGrade[i]);
		}
	}
	return NULL;

	FUNCTION_END;
}

/**
 * \brief 得到公告提示信息
 * 
 * 得到公告提示信息
 * 
 * \param action : 公告类别
 * \param bEdit : 是否可以编辑公告
 * \return 公告提示信息
 */
const stBoardShowInfo* getBoardShowInfo(enumGuiActionType  action,bool bEdit)
{
	FUNCTION_BEGIN;

	static int n=sizeof(g_BoardShowInfo)/sizeof(stBoardShowInfo);
	for (int i=0;i<n;i++)
	{
		if ((action==g_BoardShowInfo[i].action)&&(bEdit==g_BoardShowInfo[i].bEdit))
		{
			return &(g_BoardShowInfo[i]);
		}
	}
	return NULL;

	FUNCTION_END;
}

/**
 * \brief 得到社会关系的称号
 * 
 * 得到社会关系的称号
 * 
 * \param id : 社会关系类别
 * \param level : 关系等级
 * \return 社会关系的称号
 */
char* GetConnGrade(byte id,WORD level)
{
	FUNCTION_BEGIN;
	
	static char szConnGrade[32]={0}; 

	const stWorldConnGrade* worldConn=getWorldConn(id);
	if (worldConn==NULL)
		return "";

	int l = min(level*10,999990);

	if (level<LEVEL1)
		_snprintf(szConnGrade,32,"%s(%d)",worldConn->szGrade1,l);
	else if (level<LEVEL2)
		_snprintf(szConnGrade,32,"%s(%d)",worldConn->szGrade2,l);
	else if (level<LEVEL3)
		_snprintf(szConnGrade,32,"%s(%d)",worldConn->szGrade3,l);
	else 
		_snprintf(szConnGrade,32,"%s(%d)",worldConn->szGrade4,l);

	szConnGrade[31] = 0;

	return szConnGrade;

	//if (level<LEVEL1)
	//	strncpy(szConnGrade,worldConn->szGrade1,sizeof(szConnGrade));
	//else if (level<LEVEL2)
	//	strncpy(szConnGrade,worldConn->szGrade2,sizeof(szConnGrade));
	//else if (level<LEVEL3)
	//	strncpy(szConnGrade,worldConn->szGrade3,sizeof(szConnGrade));
	//else
	//	strncpy(szConnGrade,worldConn->szGrade4,sizeof(szConnGrade));
	//return szConnGrade;

	FUNCTION_END;
}

char* GetConnContext(byte id,char* name,WORD level)
{
	FUNCTION_BEGIN;

	static char szConnGrade[MAX_PATH]={0}; 
	
	const stWorldConnGrade* worldConn=getWorldConn(id);
	if (worldConn==NULL)
		return "";

	if (level<LEVEL1)
		sprintf(szConnGrade,"你与%s的关系为%s\n组队效果一般",name,GetConnGrade(id,level));
	else if (level<LEVEL2)
		sprintf(szConnGrade,"你与%s的关系为%s\n组队时双方物防和魔防上升3%%,一屏有效\n不同效果可叠加,相同效果取最大值",name,GetConnGrade(id,level));
	else if (level<LEVEL3)
		sprintf(szConnGrade,"你与%s的关系为%s\n组队时双方物防和魔防上升5%%,一屏有效\n不同效果可叠加,相同效果取最大值",name,GetConnGrade(id,level));
	else
		sprintf(szConnGrade,"你与%s的关系为%s\n组队时双方物防和魔防上升8%%,一屏有效\n不同效果可叠加,相同效果取最大值",name,GetConnGrade(id,level));

	

	return szConnGrade;

}
/**
 * \brief 得到社会关系的名称
 * 
 * 得到社会关系的名称
 * 
 * \param id : 社会关系类别
 * \return 得到社会关系的名称
 */
char* GetConnName(byte id)
{
	FUNCTION_BEGIN;
	
	static char szConnName[16]={0}; 

	const stWorldConnGrade* worldConn=getWorldConn(id);
	if (worldConn==NULL)
		return "";

	strncpy(szConnName,worldConn->szConn,sizeof(szConnName));

	return szConnName;

	FUNCTION_END;
}

/**
 * \brief 设置控件大小
 * 
 * 设置控件大小
 * 
 * \param pControl : 控件指针
 * \param dx : 控件要增加的宽度
 * \param dy : 控件要增加的高度
 * \return 无
 */
inline void AddControlSize(CGuiControl* pControl,int dx,int dy)
{
	FUNCTION_BEGIN;

	if(!pControl) return;
	dx += pControl->GetWidth();
	dy += pControl->GetHeight();
	pControl->SetSize(dx,dy);

	FUNCTION_END;
}

/**
 * \brief 设置控件的相对位置
 * 
 * 设置控件的相对位置
 * 
 * \param pControl : 控件指针
 * \param dx : 控件要增加的X位置
 * \param dy : 控件要增加的Y位置
 * \return 无
 */
inline void AddControlLocation(CGuiControl* pControl,int dx,int dy)
{
	FUNCTION_BEGIN;

	if(!pControl) return;
	stPointI pt = pControl->GetLocation();
	pt.x += dx;
	pt.y += dy;
	pControl->SetLocation(pt.x,pt.y);

	FUNCTION_END;
}

/**
 * \brief 设置控件的绝对位置
 * 
 * 设置控件的绝对位置
 * 
 * \param pControl : 控件指针
 * \param dx : 设置控件的X位置
 * \return 无
 */
inline void AddControlLocationByAbsoluteX(CGuiControl* pControl,int dx)
{
	FUNCTION_BEGIN;

	if(!pControl) return;

	pControl->SetLocation(dx,pControl->GetLocation().y);

	FUNCTION_END;
}

/**
 * \brief 设置列表框的主数据
 * 
 * 设置列表框的主数据
 * 
 * \param pListBox : 列表框的指针
 * \param szSubText : 列表框的名字
 * \param pData : 列表框的数据
 * \return 无
 */
inline void AddListSubItem(CGuiListBox* pListBox,char* szSubText,void* pData = NULL)
{
	FUNCTION_BEGIN;

	pListBox->AddItem(szSubText,pData);
	
	pListBox->SetItemColor(pListBox->GetItemCount()-1,0,SUBCOLOR);

	FUNCTION_END;
}


char*  GetCountryName(DWORD dwID)
{
	static char name[MAX_NAMESIZE]={0};

	name[0]=0;
	 
	CCountry::Instance()->GetCountryName(dwID,name);

	return name;

}

/**
 * \brief 设置列表框的数据
 * 
 * 设置列表框的数据
 * 
 * \param pListBox : 列表框的指针
 * \param szName : 玩家或NPC的名字
 * \param szFriend : 社会关系称号
 * \param true : 是否在线
 * \param pData : 该列表项的数据
 * \param nJob : 玩家角色类别
 * \param nNameWidth : 名字宽度
 * \return 无
 */
/*哪个list好友。是否在线。等级。职业*/
inline void AddListItem(CGuiListBox* pListBox,char* szName,char* szFriend,bool bOnline = true,WORD nLevel = 0,void* pData = NULL,WORD nJob = 0,/*DWORD exploit = 0,DWORD grace = 0,*/int nNameWidth=PLAYERNAMEWIDTH)
{
	FUNCTION_BEGIN;

	DWORD  dwItemColor = bOnline ? ITEMONLINECOLOR : ITEMOFFLINECOLOR;
	if (GetGameGuiManager()->m_guiWorldConn)
	{
		if ((GetGameGuiManager()->m_guiWorldConn->GetIsShowOffLine()==false)&&(bOnline==false))
			return;
	}
	
	if (bOnline==false)
		nLevel = 0;

	pListBox->AddItem("",pData);
	
	
	if (nJob)
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,GetCharTypeInfo(nJob).name,5,GuiListBoxColFmt_Center);
		pListBox->SetItemColor(pListBox->GetItemCount()-1,0,nJob);
		stPointI pt=GetResource(nJob,bOnline?eOnLine:eOffLine);
		pListBox->SetItemBmp(pListBox->GetItemCount()-1,0,&(stResourceLocation("data\\interfaces.gl",pt.x,pt.y)));	
	}
	else
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,"",2);
	}

	//soke 黑名单
	pListBox->SetItemText(pListBox->GetItemCount()-1,1,szName,nNameWidth);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,1,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,2,szFriend,12);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,2,FRIENDCOLOR);
	

	if( strncmp(szName,s_szCurSel,MAX_NAMESIZE) == 0 )
	{
		pListBox->SetCurItem( pListBox->GetItemCount()-1 );
	}

	FUNCTION_END;
}


inline void AddTongListItem(CGuiListBox* pListBox,stUnionRember* pData,DWORD color = ITEMONLINECOLOR)
{
	FUNCTION_BEGIN;

	DWORD  dwItemColor = (pData->byOnline!=false) ? color : ITEMOFFLINECOLOR;

	if (GetGameGuiManager()->m_guiWorldConn)
	{
		if ((GetGameGuiManager()->m_guiWorldConn->GetIsShowOffLine()==false)&&(pData->byOnline==false))
			return;
	}

	pListBox->AddItem("",pData);

	pListBox->SetItemText(pListBox->GetItemCount()-1,0,"",20);

	if (pData->occupation)
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,1,GetCharTypeInfo(pData->occupation).name,5,GuiListBoxColFmt_Center);
		pListBox->SetItemColor(pListBox->GetItemCount()-1,1,pData->occupation);
		stPointI pt=GetResource(pData->occupation,pData->byOnline?eOnLine:eOffLine);
		pListBox->SetItemBmp(pListBox->GetItemCount()-1,1,&(stResourceLocation("data\\interfaces.gl",pt.x,pt.y)));	
	}
	else
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,1,"",2);
	}

	pListBox->SetItemText(pListBox->GetItemCount()-1,2,pData->memberName,PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,2,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,3,pData->aliasname,PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,3,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,4,GetKnightName(pData->exploit / 100),PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,4,dwItemColor);


	if( strncmp(pData->memberName,s_szCurSel,MAX_NAMESIZE) == 0 )
	{
		pListBox->SetCurItem( pListBox->GetItemCount()-1 );
	}

	FUNCTION_END;
}

//soke 家族list
inline void AddSeptListItem(CGuiListBox* pListBox,stSeptRember* pData,DWORD color = ITEMONLINECOLOR)
{
	FUNCTION_BEGIN;
	
	DWORD  dwItemColor = (pData->byOnline!=false) ? color : ITEMOFFLINECOLOR;

	if (GetGameGuiManager()->m_guiWorldConn)
	{
		if ((GetGameGuiManager()->m_guiWorldConn->GetIsShowOffLine()==false)&&(pData->byOnline==0))
			return;
	}

	pListBox->AddItem("",pData);


	if (pData->occupation)
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,GetCharTypeInfo(pData->occupation).name,5,GuiListBoxColFmt_Center);
		pListBox->SetItemColor(pListBox->GetItemCount()-1,0,pData->occupation);
		stPointI pt=GetResource(pData->occupation,(pData->byOnline!=0)?eOnLine:eOffLine);
		pListBox->SetItemBmp(pListBox->GetItemCount()-1,0,&(stResourceLocation("data\\interfaces.gl",pt.x,pt.y)));	
	}
	else
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,"",2);
	}

	pListBox->SetItemText(pListBox->GetItemCount()-1,1,pData->memberName,PLAYERNAMEWIDTH+6);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,1,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,2,pData->memberAliasName,PLAYERNAMEWIDTH+15);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,2,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,3,GetKnightName(pData->exploit / 100),PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,3,dwItemColor);

	if( strncmp(pData->memberName,s_szCurSel,MAX_NAMESIZE) == 0 )
	{
		pListBox->SetCurItem( pListBox->GetItemCount()-1 );
	}

	FUNCTION_END;
}


//师傅
inline void AddTeacherListItem(CGuiListBox* pListBox,stTeacherMemberEx* pData,char* szFriend)
{
	FUNCTION_BEGIN;

	DWORD  dwItemColor = pData->online ? ITEMONLINECOLOR : ITEMOFFLINECOLOR;
	if (GetGameGuiManager()->m_guiWorldConn)
	{
		if ((GetGameGuiManager()->m_guiWorldConn->GetIsShowOffLine()==false)&&(pData->online==false))
			return;
	}

	pListBox->AddItem("",pData);


	if (pData->occupation)
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,GetCharTypeInfo(pData->occupation).name,5,GuiListBoxColFmt_Center);
		pListBox->SetItemColor(pListBox->GetItemCount()-1,0,pData->occupation);
		stPointI pt=GetResource(pData->occupation,pData->online?eOnLine:eOffLine);
		pListBox->SetItemBmp(pListBox->GetItemCount()-1,0,&(stResourceLocation("data\\interfaces.gl",pt.x,pt.y)));	
	}
	else
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,"",2);
	}

	pListBox->SetItemText(pListBox->GetItemCount()-1,1,pData->name,PLAYERNAMEWIDTH+3);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,1,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,2,GetCountryName(pData->country),12);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,2,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,3,pData->septName,PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,3,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,4,pData->unionName,PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,4,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,5,szFriend,12);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,5,dwItemColor);


	if( strncmp(pData->name,s_szCurSel,MAX_NAMESIZE) == 0 )
	{
		pListBox->SetCurItem( pListBox->GetItemCount()-1 );
	}

	FUNCTION_END;
}


//好友的ITEM
inline void AddFriendListItem(CGuiListBox* pListBox,stRelation* pData,char* szFriend)
{
	FUNCTION_BEGIN;

	DWORD  dwItemColor = pData->online ? ITEMONLINECOLOR : ITEMOFFLINECOLOR;
	if (GetGameGuiManager()->m_guiWorldConn)
	{
		if ((GetGameGuiManager()->m_guiWorldConn->GetIsShowOffLine()==false)&&(pData->online==false))
			return;
	}

	pListBox->AddItem("",pData);


	if (pData->occupation)
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,GetCharTypeInfo(pData->occupation).name,5,GuiListBoxColFmt_Center);
		pListBox->SetItemColor(pListBox->GetItemCount()-1,0,pData->occupation);
		stPointI pt=GetResource(pData->occupation,pData->online?eOnLine:eOffLine);
		pListBox->SetItemBmp(pListBox->GetItemCount()-1,0,&(stResourceLocation("data\\interfaces.gl",pt.x,pt.y)));	
	}
	else
	{
		pListBox->SetItemText(pListBox->GetItemCount()-1,0,"",2);
	}

	pListBox->SetItemText(pListBox->GetItemCount()-1,1,pData->name,PLAYERNAMEWIDTH+3);
//soke list  好友的放置位置
	pListBox->SetItemColor(pListBox->GetItemCount()-1,1,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,2,GetCountryName(pData->country),12);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,2,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,3,pData->unionName,PLAYERNAMEWIDTH);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,3,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,4,GetKnightName(pData->exploit / 100),16);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,4,dwItemColor);

	pListBox->SetItemText(pListBox->GetItemCount()-1,5,szFriend,16);
	pListBox->SetItemColor(pListBox->GetItemCount()-1,5,dwItemColor);

	if( strncmp(pData->name,s_szCurSel,MAX_NAMESIZE) == 0 )
	{
		pListBox->SetCurItem( pListBox->GetItemCount()-1 );
	}

	FUNCTION_END;
}
/**
 * \brief 刷新社会关系头像
 * 
 * 刷新社会关系头像
 * 
 * \param pListBox : 列表控件指针
 * \param state : 社会关系类别
 * \return NULL
 */
inline void RefreshPrivate(CGuiListBox* pListBox,byte state)
{
	FUNCTION_BEGIN;

//	GetChatManager().RefreshWorldConnListBox(pListBox,state);

	FUNCTION_END;
}


/**
 * \brief 社会关系类构造函数
 * 
 * 社会关系类构造函数
 * 
 * \return 无
 */
CGuiWorldConn::CGuiWorldConn(void)
:m_pScrollTab(NULL)
,m_bStartSize(false)
,m_pTongMenu(NULL)
,m_pRelationMenu(NULL)
,m_pSchoolMenu(NULL)
,m_pNearlyMenu(NULL)
,m_pSelData(NULL)
,m_bPlaySound(false)
,m_pOthermenu(NULL)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;
	m_pChannel = NULL;
	m_minSize.x = 290;
	m_minSize.y = 580;
	memset(m_szSelectRember,0,sizeof(m_szSelectRember));
	
	m_TabList.clear();
	m_boardList.clear();

	FUNCTION_END;
}

/**
 * \brief 社会关系类析构函数
 * 
 * 社会关系类析构函数
 * 
 * \return 无
 */
CGuiWorldConn::~CGuiWorldConn(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 设置关系按钮位置
 * 
 * 设置关系按钮位置
 * 
 * \return 无
 */
void CGuiWorldConn::OnSetCloseLocation()
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
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiWorldConn::InitCustomElements()
{
	FUNCTION_BEGIN;

	/*
	const char* szGrpFileName = GetGuiGraphicPackName();

	CGuiElement Element;
	stRectI rcTexture;
	stRectI rects[9];

	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 0;
	rl.frame = 0;
	
	stRectI m_borderRect =  stRectI(0,0,256,256);
	//-------------------------------------
	// Element for the back 外框
	//-------------------------------------
	m_BackElements.resize(9);
	{
		rl.group = 11;
		rl.frame = 0;
		CuttRect9(m_borderRect,m_borderTopLeft,m_borderRightBottom,rects);
		for(size_t i=0;i<count_of(rects);++i)
		{
			CGuiElement* pEmt = new CGuiElement;
			pEmt->SetTexture(&rl,&rects[i]);
			pEmt->FontColor.Current = COLOR_ARGB(255,255,255,255);
			pEmt->TextureColor.Current = COLOR_ARGB(255,255,255,255);
			m_BackElements[i] = pEmt;
		}
	}
	*/


	FUNCTION_END;
}

/**
 * \brief 重画社会关系对话框
 * 
 * 重画社会关系对话框
 * 
 * \param fElapsedTime : 这贞与上贞的时间
 * \return 是否成功
 */
HRESULT CGuiWorldConn::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	static DWORD m_dwRefreshTime=0;
	
	//checkBoard();

	if(xtimeGetTime() - m_dwRefreshTime > 5000)
	{
		m_dwRefreshTime = xtimeGetTime();
		RefreshCharacter();
	}

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	BYTE  nSelectId = m_pScrollTab->GetCurItem();

	switch(nSelectId) 
	{
	case FRIEND:
		{
			stRelation*  pData =(stRelation*) GetListBox(FriendList)->GetItemData(GetListBox(FriendList)->GetMoveSel());
			if (pData)
			{
				stRectI rect=GetListBox(FriendList)->GetMoveSelRect();
				rect.OffsetRect(GetClientX(),GetClientY());
					
				m_ToolTips.Clear();
				m_ToolTips.SetCurColor( 0xFFFFFFFF );
				m_ToolTips.AddText(GetConnContext(pData->type,pData->name,pData->level));
				m_ToolTips.Resize();

				m_ToolTips.RenderTopLeftPrefer(rect,GetDevice()->GetWidth(),GetDevice()->GetHeight());
			}
		}
		break;
	case NEARBY:
		{
		}
		break;
	case NEARBYNPC:
		{
			stPointI ptCursor = Engine_GetCursor()->GetPosition();
			ptCursor.x -= GetClientX();
			ptCursor.y -= GetClientY();
			stRectI rcClient(110,120+40,357-90,140+40);
			if ( rcClient.PtInRect(ptCursor) )
			{
				m_ToolTips.Clear();
				m_ToolTips.SetCurColor( 0xFFFFFFFF );
				m_ToolTips.AddText("在玩家列表或NPC列表里查找目标时,可按照目标首字母使用键盘对应字母快速查找 ");
				m_ToolTips.Resize();
				m_ToolTips.RenderTopLeftPrefer(rcClient,GetDevice()->GetWidth(),GetDevice()->GetHeight());
				stPointI ptTips;
				ptTips = Engine_GetCursor()->GetPosition();
				ptTips.y -= 60;
                //if ( NULL == GetGuiManager()->GetToolTips() )
				{
					GetGuiManager()->SetToolTips( &m_ToolTips, ptTips );
				}				
			}
			stRectI rcClient_NPCName(50,206,214,467);
			if ( rcClient_NPCName.PtInRect(ptCursor) )
			{
				m_ToolTips.Clear();
				m_ToolTips.SetCurColor( 0xFFFFFFFF );
				m_ToolTips.AddText("双击该NPC,则自动寻路到该NPC处");
				m_ToolTips.Resize();
				m_ToolTips.RenderTopLeftPrefer(rcClient,GetDevice()->GetWidth(),GetDevice()->GetHeight());
				stPointI ptTips;
				ptTips = Engine_GetCursor()->GetPosition();
				ptTips.y -= 60;
				//if ( NULL == GetGuiManager()->GetToolTips() )
				{
					GetGuiManager()->SetToolTips( &m_ToolTips, ptTips );
				}

				stResourceLocation rl("data\\cursor.gl",0,14);
				//换鼠标ico
				GetGameCursor()->SetCursor( CCursor::CursorType_Select, eCursorNone );
				GetGameCursor()->SetImage( &rl, stPointI(12,1) );
			}
			else
			{
				stResourceLocation rl("data\\cursor.gl",0,0);
				//换鼠标ico
				GetGameCursor()->SetCursor( CCursor::CursorType_Select, eCursorNone );
				GetGameCursor()->SetImage( &rl, stPointI(1,1) );
			}

		}
		break;
	}

	return hr;

	FUNCTION_END;
}

/**
 * \brief 检查是否留言数据
 * 
 * 检查是否留言数据
 * 
 * \return 无
 */
void  CGuiWorldConn::checkBoard()
{
	FUNCTION_BEGIN;

	static BOARDINFOLIST::iterator pointer;
	BYTE  nSelectId = m_pScrollTab->GetCurItem();
	
	switch(nSelectId) {
	case TONG:
		{
			GetButton(TongButtonW)->SetVisible(false);
			stBoardInfo* pInfo=(stBoardInfo*)GetListItem(m_boardList,CHAT_TYPE_UNION_AFFICHE);
			if (pInfo)
			{
				if (pInfo->update)
					GetButton(TongButtonW)->SetVisible(true);
			}
			
		}
		break;
	case SEPT:
		{
			GetButton(SeptButtonW)->SetVisible(false);
			stBoardInfo* pInfo=(stBoardInfo*)GetListItem(m_boardList,CHAT_TYPE_FAMILY_AFFICHE);
			if (pInfo)
			{
				if (pInfo->update)
					GetButton(SeptButtonW)->SetVisible(true);
			}
		}
		break;
	case SCHOOL:
		{
			GetButton(SchoolButtonW)->SetVisible(false);
			stBoardInfo* pInfo=(stBoardInfo*)GetListItem(m_boardList,CHAT_TYPE_OVERMAN_AFFICHE);
			if (pInfo)
			{
				if (pInfo->update)
					GetButton(SchoolButtonW)->SetVisible(true);
			}
		}
		break;
	case FRIEND:
		{
			
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 创建社会关系对话框
 * 
 * 创建社会关系对话框
 * 
 * \return 无
 */
void CGuiWorldConn::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_bCaption=false;

	//*
	m_curDlgSize.cx =	GetWidth();
	m_curDlgSize.cy =	GetHeight();
	m_pScrollTab =  GetScrollTab(SCROLLTAB);
	m_pTongEditBoard = GetMLEditBox(TongEditBoard);
	m_pSeptEditBoard = GetMLEditBox(SeptEditBoard);
	m_pSchoolEditBoard = GetMLEditBox(SchoolEditBoard);

	if ( m_pTongEditBoard )
		m_pTongEditBoard->SetMaxCharCount( 254 );
	if ( m_pSeptEditBoard )
		m_pSeptEditBoard->SetMaxCharCount( 254 );
	if (  m_pSchoolEditBoard )
		m_pSchoolEditBoard->SetMaxCharCount( 254 );
	
	
	m_pTongMenu = GetGuiManager()->CreateMenu(NULL);
	m_pSeptMenu = GetGuiManager()->CreateMenu(NULL);
	m_pRelationMenu = GetGuiManager()->CreateMenu(NULL);
	m_pSchoolMenu = GetGuiManager()->CreateMenu(NULL);
	m_pNearlyMenu = GetGuiManager()->CreateMenu(NULL);
	m_pNearlyNpcMenu = GetGuiManager()->CreateMenu(NULL);
	m_pBadUserMenu = GetGuiManager()->CreateMenu(NULL);
	m_pEnemyMenu = GetGuiManager()->CreateMenu(NULL);
	m_pOthermenu = GetGuiManager()->CreateMenu(NULL);

	AddMenuItem(TONG,m_pTongMenu);
	AddMenuItem(SEPT,m_pSeptMenu);
	AddMenuItem(SCHOOL,m_pSchoolMenu);
	AddMenuItem(FRIEND,m_pRelationMenu);
	AddMenuItem(NEARBY,m_pNearlyMenu);
	AddMenuItem(NEARBYNPC,m_pNearlyNpcMenu);
	AddMenuItem(BADUSER,m_pBadUserMenu);
	AddMenuItem(ENEMYUSER,m_pEnemyMenu);
	AddMenuItem(OTHER,m_pOthermenu);
	
	stTabInfo st;

	st.pMenu = m_pTongMenu;
	st.pListBox = GetListBox(TongList);
	st.ePrivateType = CHAT_TYPE_UNION_PRIVATE;
	st.eBoardType = CHAT_TYPE_UNION_AFFICHE;
	InsertListItem(&st,m_TabList,TONG);
	
	st.pMenu = m_pSeptMenu;
	st.pListBox = GetListBox(SeptList);
	st.ePrivateType = CHAT_TYPE_FAMILY_PRIVATE;
	st.eBoardType = CHAT_TYPE_FAMILY_AFFICHE;
	InsertListItem(&st,m_TabList,SEPT);

	st.pMenu = m_pSchoolMenu;
	st.pListBox = GetListBox(SchoolList);
	st.ePrivateType = CHAT_TYPE_OVERMAN_PRIVATE;
	st.eBoardType = CHAT_TYPE_OVERMAN_AFFICHE;
	InsertListItem(&st,m_TabList,SCHOOL);

	st.pMenu = m_pRelationMenu;
	st.pListBox = GetListBox(FriendList);
	st.ePrivateType = CHAT_TYPE_FRIEND_PRIVATE;
	st.eBoardType = CHAT_TYPE_FRIEND_AFFICHE;
	InsertListItem(&st,m_TabList,FRIEND);

	st.pMenu = m_pNearlyMenu;
	st.pListBox = GetListBox(NearbyList);
	st.ePrivateType = CHAT_TYPE_PRIVATE;
	st.eBoardType = 0;
	InsertListItem(&st,m_TabList,NEARBY);

	st.pMenu = m_pNearlyNpcMenu;
	st.pListBox = GetListBox(NearbyNpcList);
	st.ePrivateType = 0;
	st.eBoardType = 0;
	InsertListItem(&st,m_TabList,NEARBYNPC);

	st.pMenu = m_pBadUserMenu;
	st.pListBox = GetListBox(BadUserList);
	st.ePrivateType = 0;
	st.eBoardType = 0;
	InsertListItem(&st,m_TabList,BADUSER);

	st.pMenu = m_pEnemyMenu;
	st.pListBox = GetListBox(ENEMYList);
	st.ePrivateType = 0;
	st.eBoardType = 0;
	InsertListItem(&st,m_TabList,ENEMYUSER);

	m_pScrollTab->SetVisible(false);

	m_pScrollTab->AddControl(TONG,GetImage(TongBack));
	m_pScrollTab->AddControl(TONG,GetListBox(TongList));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton1));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton2));
	m_pScrollTab->AddControl(TONG,GetStatic(TongStatic));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton3));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton4));
	//m_pScrollTab->AddControl(TONG,GetButton(TongButton5));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton6));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton7));
	m_pScrollTab->AddControl(TONG,GetButton(TongButton8));
	m_pScrollTab->AddControl(TONG,GetButton(310));	//帮会聊天
	m_pScrollTab->AddControl(TONG,GetButton(3119)); //帮会介绍
	m_pScrollTab->AddControl(TONG,GetStatic(311));
	m_pScrollTab->AddControl(TONG,GetStatic(312));
	m_pScrollTab->AddControl(TONG,GetStatic(313));
	m_pScrollTab->AddControl(TONG,m_pTongEditBoard);
	//m_pScrollTab->AddControl(TONG,GetButton(TongButtonW));
	
	m_pScrollTab->AddControl(SEPT,GetImage(SeptBack));
	m_pScrollTab->AddControl(SEPT,GetListBox(SeptList));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton1));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton2));
	m_pScrollTab->AddControl(SEPT,GetStatic(SeptStatic));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton3));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton4));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton8));
	m_pScrollTab->AddControl(SEPT,GetButton(SeptButton9));
	m_pScrollTab->AddControl(SEPT,GetButton(212));	//家族聊天
	m_pScrollTab->AddControl(SEPT,GetButton(2139)); //家族介绍
	m_pScrollTab->AddControl(SEPT,GetStatic(207));
	m_pScrollTab->AddControl(SEPT,GetStatic(208));
	m_pScrollTab->AddControl(SEPT,GetStatic(209));
	m_pScrollTab->AddControl(SEPT,m_pSeptEditBoard);
	//m_pScrollTab->AddControl(SEPT,GetButton(SeptButtonW));
	
	m_pScrollTab->AddControl(SCHOOL,GetImage(SchoolBack));
	m_pScrollTab->AddControl(SCHOOL,GetListBox(SchoolList));
	m_pScrollTab->AddControl(SCHOOL,GetButton(SchoolButton1));
	m_pScrollTab->AddControl(SCHOOL,GetButton(SchoolButton2));
	m_pScrollTab->AddControl(SCHOOL,GetStatic(SchoolStatic));
	m_pScrollTab->AddControl(SCHOOL,GetButton(SchoolButton3));
	m_pScrollTab->AddControl(SCHOOL,GetButton(SchoolButton4));
	m_pScrollTab->AddControl(SCHOOL,m_pSchoolEditBoard);
	//m_pScrollTab->AddControl(SCHOOL,GetButton(SchoolButtonW));
	
	m_pScrollTab->AddControl(FRIEND,GetImage(FriendBack));
	m_pScrollTab->AddControl(FRIEND,GetListBox(FriendList));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton1));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton2));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton3));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton4));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton5));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton6));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton7));
	m_pScrollTab->AddControl(FRIEND,GetButton(FriendButton10));
	m_pScrollTab->AddControl(FRIEND,GetButton(FRIENDPAGE));
	m_pScrollTab->AddControl(FRIEND,GetButton(BADPAGE));
	m_pScrollTab->AddControl(FRIEND,GetButton(ENEMYPAGE));
	m_pScrollTab->AddControl(FRIEND,GetButton(CARTOON_PET));

	
	m_pScrollTab->AddControl(NEARBY,GetImage(NearbyBack));
	m_pScrollTab->AddControl(NEARBY,GetListBox(NearbyList));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton1));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton2));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton3));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton4));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton5));
	m_pScrollTab->AddControl(NEARBY,GetButton(NearbyButton6));
	m_pScrollTab->AddControl(NEARBY,GetButton(NEARBYPAGE));
	m_pScrollTab->AddControl(NEARBY,GetButton(NEARBYNPCPAGE));
	
	m_pScrollTab->AddControl(NEARBYNPC,GetImage(NearbyNpcBack));
	m_pScrollTab->AddControl(NEARBYNPC,GetListBox(NearbyNpcList));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton1));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton2));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton3));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton4));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton5));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NearbyNpcButton6));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NEARBYPAGE));
	m_pScrollTab->AddControl(NEARBYNPC,GetButton(NEARBYNPCPAGE));
	
	m_pScrollTab->AddControl(BADUSER,GetImage(BadUserBack));
	m_pScrollTab->AddControl(BADUSER,GetListBox(BadUserList));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton1));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton2));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton3));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton4));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton5));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton6));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton7));
	m_pScrollTab->AddControl(BADUSER,GetButton(BadUserButton8));
	m_pScrollTab->AddControl(BADUSER,GetButton(FRIENDPAGE));
	m_pScrollTab->AddControl(BADUSER,GetButton(BADPAGE));
	m_pScrollTab->AddControl(BADUSER,GetButton(ENEMYPAGE));
	m_pScrollTab->AddControl(BADUSER,GetButton(CARTOON_PET));


	m_pScrollTab->AddControl(ENEMYUSER,GetImage(ENEMYBack));
	m_pScrollTab->AddControl(ENEMYUSER,GetListBox(ENEMYList));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton1));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton2));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton3));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton4));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton5));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton6));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton7));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYButton8));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(FRIENDPAGE));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(BADPAGE));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(ENEMYPAGE));
	m_pScrollTab->AddControl(ENEMYUSER,GetButton(CARTOON_PET));



	GetButton(FRIENDBUTTON)->SetButtonGroup(1);
	   GetButton(SEPTBUTTON)->SetButtonGroup(1);
	     GetButton(TONGBUTTON)->SetButtonGroup(1);
	//GetButton(NEARYBYBUTTON)->SetButtonGroup(1);
	if (GetButton(TEACHERBUTTON))
		GetButton(TEACHERBUTTON)->SetButtonGroup(1);
	if( GetButton(ID_BUTTON_COUNTRY))
		GetButton(ID_BUTTON_COUNTRY)->SetButtonGroup(1);


	GetButton(FRIENDPAGE)->SetButtonGroup(2);
	GetButton(BADPAGE)->SetButtonGroup(2);

	if (GetButton(ENEMYPAGE))
		GetButton(ENEMYPAGE)->SetButtonGroup(2);

	//GetButton( CARTOON_PET )->SetButtonGroup(2);	


	GetButton(NEARBYPAGE)->SetButtonGroup(3);
	GetButton(NEARBYNPCPAGE)->SetButtonGroup(3);



	//m_pScrollTab->SetCurItem(0);
	OnGuiEvent(EVENT_BUTTON_CLICKED,FRIENDBUTTON,NULL);
	m_clientOffsetTopLeft.x=0;
	m_clientOffsetRightBottom.x=0;

	m_bStartSize=true;
	OnGuiEvent(EVENT_TAB_SELECTION_CHANGED,0,NULL);
	//OnGuiEvent(EVENT_DIALOG_RESIZE,0,NULL);
	//*/

	stBoardInfo stBoard;
	stBoard.update=false;
	
	InsertListItem(&stBoard,m_boardList,CHAT_TYPE_FRIEND_AFFICHE);
	InsertListItem(&stBoard,m_boardList,CHAT_TYPE_UNION_AFFICHE);
	InsertListItem(&stBoard,m_boardList,CHAT_TYPE_OVERMAN_AFFICHE);
	InsertListItem(&stBoard,m_boardList,CHAT_TYPE_FAMILY_AFFICHE);
	
	CGuiCheckBox* pCheckBox = GetCheckBox(1);
	if(pCheckBox)
		m_bShowOffline = pCheckBox->GetChecked();
	
	InitTong();
	InitSept();
	InitRational();
	InitSchool();
	InitBadUser();
	InitEnemy();

	m_NearlyList.resize(maxNearNum);

#ifdef _DEBUG
	RefreshCharacter();
#endif

	FUNCTION_END;
}

bool CGuiWorldConn::IsSelectItem()
{
	if (m_pSelData==NULL)
	{
		GameMessageBox("请先选中一位玩家！");
		return false;
	}

	return true;
}

bool  CGuiWorldConn::IsTeamer(const char* name,bool bShow)
{
	stUnionRember* pSt=(stUnionRember*)GetListItem(m_TongList,name);
	if ((pSt)&&(pSt->bySeptMaster==1))
	{
		return true;
	}
	if (bShow)
		GameMessageBox("请先选中一位族长进行操作！");
	return false;
}

int CGuiWorldConn::GetTongSeptMasterNum()
{
	int num = 0;
	stUnionRember* pRember = NULL;
	for (TONGLIST::iterator pointer=m_TongList.begin();pointer!=m_TongList.end();pointer++)
	{
		pRember = &(pointer->second);
		if ( pRember && (pRember->bySeptMaster == 1) )
			num++ ;
	}
	return num;
}

/**
 * \brief 重画对话框背景
 * 
 * 重画对话框背景
 * 
 * \param fElapsedTime : 重画时间
 * \return 无
 */
void CGuiWorldConn::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	CGuiDialog::RenderBackground(fElapsedTime);

	FUNCTION_END;
}

/**
* \brief 重新设置组队菜单的状态
* 
* \param pMenu : 要设置的菜单
* \param pBtn :  菜单中组队按钮id表
* \param BtnSize: 菜单中组队按钮列表的个数
* \return 无
*/
void CGuiWorldConn::UpdateMakeGroupMenuState(CGuiMenu * pMenu,int * pBtn,int BtnSize)
{
     FUNCTION_BEGIN;
      
#ifdef  _DEBUG_GROUP
     assert(BtnSize == 3);
     if( BtnSize < 3)    return;
#else 
     assert(BtnSize == 2);
     if( BtnSize < 2)    return;
#endif 

     SetMenuEnabled(pMenu,pBtn[0],true);  // 普通组队
     CCharacter * pSelCharacter = GetScene()->FindCharacterByName(m_szSelectRember);

     bool bCanMakeHonorGroup = false;
     if( !pSelCharacter)
	     bCanMakeHonorGroup = false;
     else 
	     bCanMakeHonorGroup = CanMakeGroupWithMe(pSelCharacter->GetLevel(),TEAM_HONOR);

     SetMenuEnabled(pMenu,pBtn[1],bCanMakeHonorGroup); // 荣毅组队
#ifdef _DEBUG_GROUP
     SetMenuEnabled(pMenu,pBtn[2],true); // 特殊组队
#endif 

     FUNCTION_END;
}
/**
 * \brief 重新设置菜单
 * 
 * 重新设置菜单
 * 
 * \return 无
 */
bool CGuiWorldConn::OnContextMenu()
{
	FUNCTION_BEGIN;

	BYTE  nSelectId = m_pScrollTab->GetCurItem();

	SetMenuDisenable(nSelectId,GetPopupMenu());
	m_pSelData=ResetSelectRember(GetCurrListBox(),m_szSelectRember);

	switch(nSelectId) {
	case TONG:     // 帮会
		{
			if (isset_state(m_TongPower,ADD_MEMBER))
			{
				SetMenuEnabled(m_pTongMenu,TongButton6,true);
			}
			

			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				if (isset_state(m_TongPower,CHANGE_ALIAS))
				{
					SetMenuEnabled(m_pTongMenu,TongButton3,true);
					
				}

				//if (isset_state(m_TongPower,MASTER))
				

				if (isset_state(m_TongPower,ADD_MEMBER))
				{
					SetMenuEnabled(m_pTongMenu,TongButton6,true);
				}

				if (isset_state(m_TongPower,FIRE_MEMBER))
				{
					SetMenuEnabled(m_pTongMenu,TongButton5,true);
				}

				if (getSelectType(m_TongInfo.master)==eSelectMine)
				{
					SetMenuEnabled(m_pTongMenu,TongButton3,true);
					SetMenuEnabled(m_pTongMenu,TongButton4,true);
					SetMenuEnabled(m_pTongMenu,TongButton5,true);
					SetMenuEnabled(m_pTongMenu,TongButton6,true);
				}

				SetMenuEnabled(m_pTongMenu,TongButton100,true);

			}
			

			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				//soke 右键菜单
				SetMenuEnabled(m_pTongMenu,TongButton7,true);

				// 加为好友，组队菜单
				SetMenuEnabled(m_pTongMenu,TongButton8,true);  // friend

				int nBtn[] =
				{
					TongButton9,
					TongButton10,
#ifdef _DEBUG_GROUP
					TongButton11,
#endif 
				};
				UpdateMakeGroupMenuState(m_pTongMenu,nBtn,count_of(nBtn));
				
				SetMenuEnabled(m_pTongMenu,TongButton12,true);  
				SetMenuEnabled(m_pTongMenu,TongButton13,true);  
			}			
		}
		break;
	case SEPT:	// 家族
		{
			if (getSelectType(m_SeptInfo.master)==eSelectMine)
			{
				if (getSelectType(m_szSelectRember)==eSelectRember)
				{
					SetMenuEnabled(m_pSeptMenu,SeptButton3,true);
				}
				SetMenuEnabled(m_pSeptMenu,SeptButton4,true);
			}
			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				SetMenuEnabled(m_pSeptMenu,SeptButton5,true);
				SetMenuEnabled(m_pSeptMenu,SeptButton100,true);

				// 加为好友，组队菜单
				SetMenuEnabled(m_pSeptMenu,SeptButton6,true);  // friend 

				int nBtn[] =
				{
					SeptButton7,
					SeptButton8,
#ifdef _DEBUG_GROUP
					SeptButton9,
#endif 
				};
				UpdateMakeGroupMenuState(m_pSeptMenu,nBtn,count_of(nBtn));
				
				SetMenuEnabled(m_pSeptMenu,SeptButton10,true);  
				SetMenuEnabled(m_pSeptMenu,SeptButton11,true); 
			}
		}
		break;
	case SCHOOL:
		{
			SetMenuEnabled(m_pSchoolMenu,SchoolButton3,true);
			SetMenuEnabled(m_pSchoolMenu,SchoolButton6,true);

			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				SetMenuEnabled(m_pSchoolMenu,SchoolButton100,true);
				SetMenuEnabled(m_pSchoolMenu,SchoolButton4,true);
				SetMenuEnabled(m_pSchoolMenu,SchoolButton5,true);
			}
		}
		break;
	case FRIEND:		// 好友
		{
			SetMenuEnabled(m_pRelationMenu,FriendButton3,true);

			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				//soke 右键菜单
				SetMenuEnabled(m_pRelationMenu,FriendButton100,true);
				SetMenuEnabled(m_pRelationMenu,FriendButton15,true);
				void *pFriend = GetListItem(m_RelationList,m_szSelectRember);
				if (pFriend)
				{
					SetMenuEnabled(m_pRelationMenu,FriendButton4,true);
					SetMenuEnabled(m_pRelationMenu,FriendButton5,true);
					SetMenuEnabled(m_pRelationMenu,FriendButton14,true);
					//SetMenuEnabled(m_pRelationMenu,FriendButton101,true);					
				}

				void *pBad = GetListItem(m_BadList,m_szSelectRember);
				if (pBad)
				{
					//SetMenuEnabled(m_pRelationMenu,BadUserButton2,true);
				}

				SetMenuEnabled(m_pRelationMenu,FriendButton8,true);
				SetMenuEnabled(m_pRelationMenu,FriendButton9,true);
				
				// 组队菜单
				int nBtn[] =
				{
					FriendButton11,
					FriendButton12,
#ifdef _DEBUG_GROUP
					FriendButton13,
#endif
				};
				UpdateMakeGroupMenuState(m_pRelationMenu,nBtn,count_of(nBtn));

				//SetMenuEnabled(m_pRelationMenu,FriendButton14,true);
			}
		}
		break;
	case BADUSER:
		{
			SetMenuEnabled(m_pBadUserMenu,BadUserButton1,true);
			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				void *pBad = GetListItem(m_BadList,m_szSelectRember);
				if (pBad)
				{
					SetMenuEnabled(m_pBadUserMenu,BadUserButton2,true);
				}

				SetMenuEnabled(m_pBadUserMenu,BadUserButton3,true);
				SetMenuEnabled(m_pBadUserMenu,BadUserButton100,true);
				SetMenuEnabled(m_pBadUserMenu,BadUserButton9,true);
			}
		}
		break;
	case NEARBY:
		{
			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				SetMenuEnabled(m_pNearlyMenu,NearbyButton100,true);
				SetMenuEnabled(m_pNearlyMenu,NearbyButton10,true);

				void *p = GetListItem(m_RelationList,m_szSelectRember);
				if (p)
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton6,true);
				}
				else
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton1,true);
				}

				p = GetListItem(m_BadList,m_szSelectRember);
				if (p==NULL)
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton7,true);
				}

				p = GetListItem(m_TeacherList,m_szSelectRember);
				if (p==NULL)
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton5,true);
				}
	
				if (strlen(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName)>0)
				{
					if (getSelectType(GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.master)==eSelectMine)
						SetMenuEnabled(m_pNearlyMenu,NearbyButton3,true);					
				}

				if (strlen(GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName)>0)
				{
					if (isset_state(GetGameGuiManager()->m_guiWorldConn->m_TongPower,ADD_MEMBER))
					{
						SetMenuEnabled(m_pNearlyMenu,NearbyButton4,true);
					}
				}
				
				if (!GetGameGuiManager()->m_guiTeam->FindMember(m_szSelectRember))
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton2,true);
				}
				
				SetMenuEnabled(m_pNearlyMenu,NearbyButton8,true);

				SetMenuEnabled(m_pNearlyMenu,NearbyButton9,false);
				CCharacter* pChar = GetScene()->FindCharacterByName(m_szSelectRember);
				if ( pChar && pChar->bIsShopStart() )
				{
					SetMenuEnabled(m_pNearlyMenu,NearbyButton9,true);
				}				
			}
		}
		break;
	case NEARBYNPC:
		{
			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				SetMenuEnabled(m_pNearlyNpcMenu,NearbyNpcButton1,true);
			}
		}
		break;
	case ENEMYUSER:
		{
			if (getSelectType(m_szSelectRember)==eSelectRember)
			{
				SetMenuEnabled(m_pEnemyMenu,ENEMYButton1,true);
				SetMenuEnabled(m_pEnemyMenu,ENEMYButton2,true);
				SetMenuEnabled(m_pEnemyMenu,ENEMYButton100,true);
				SetMenuEnabled(m_pEnemyMenu,ENEMYButton101,true);
			}
		}
		break;
	}
	
	return CGuiDialog::OnContextMenu();

	FUNCTION_END;
}

bool  CGuiWorldConn::IsMyRelation(const char* szName,BYTE state)
{
	void* p = NULL;

	switch(state) {
	case TONG:
		p = GetListItem(m_TongList,szName);
		break;
	case SEPT:
		p = GetListItem(m_SeptList,szName);
		break;
	case FRIEND:
		p = GetListItem(m_RelationList,szName);
		break;
	case BADUSER:
		p = GetListItem(m_BadList,szName);
		break;
	case ENEMYUSER:
		p = GetListItem(m_EnemyList,szName);
		break;
	case  RELATE_CONSORT:
		p=  GetListItem(m_ConsortList,szName);			//刷新
		break;
	}
	if (p==NULL)
		return false;

	return true;
}

/**
 * \brief 得到控件位置与宽度的比例
 * 
 * 得到控件位置与宽度的比例
 * 
 * \param pControl : 控件指针
 * \return 控件位置与宽度的比例
 */
float CGuiWorldConn::getRate(CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	float* p = (float*)GetListItem(m_rateList,pControl->GetID());
	if (p)
		return (*p);
	
	stPointI pt = pControl->GetLocation();
	float rate =(float) pt.x / GetWidth();
	InsertListItem(&rate,m_rateList,pControl->GetID());
	return rate;

	FUNCTION_END;
}

/**
 * \brief 设置当前的社会关系项
 * 
 * 设置当前的社会关系项
 * 
 * \param Index : 要设置当前的社会关系项
 * \return 无
 */
void CGuiWorldConn::ActiveTabItem(BYTE Index)
{
	FUNCTION_BEGIN;

	if (m_pScrollTab)
		m_pScrollTab->SetCurItem( Index );

	FUNCTION_END;
}

/**
 * \brief 得到当前的社会关系项
 * 
 * 得到当前的社会关系项
 * 
 * \return 得到当前的社会关系项
 */
int CGuiWorldConn::GetActiveTabItem()
{
	FUNCTION_BEGIN;

	if ( m_pScrollTab )
		return m_pScrollTab->GetCurItem();

	return -1;

	FUNCTION_END;
}

void CGuiWorldConn::SendBoard(const char* pStr,byte eState)
{
	stChannelChatUserCmd cmd;

	cmd.dwType = eState;
	strncpy(cmd.pstrName, GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.pstrName));
	strncpy(cmd.pstrChat,pStr,sizeof(cmd.pstrChat));

	SEND_USER_CMD(cmd);
}

bool CGuiWorldConn::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( (GetKeyState(VK_CONTROL) < 0) && (wParam == 'C') )
	{
		if( IsSelectItem() )
		{
			Copy2Clipboard( m_szSelectRember );
			return true;
		}
	}

	if ( wParam >= 'A' && wParam <= 'Z' && uMsg == WM_KEYDOWN &&
		this == GetGameGuiManager()->m_guiNearby )	//附近玩家
	{
		char c = (char)wParam;
#ifdef _DEBUG
		AddClientSystemMessageF("WorldConn Press %c",c);
#endif
		if ( wParam == 'F' )
		{
			stPointI ptCursor = Engine_GetCursor()->GetPosition();
			ptCursor.x -= GetClientX();
			ptCursor.y -= GetClientY();
			stRectI rcClient = GetClipRect();
			if ( !rcClient.PtInRect(ptCursor) )
			{
				SetVisible(false);
				return true;
			}			
		}
		CGuiListBox* pListBox = GetCurrListBox();
		if( pListBox )
		{
			pListBox->FindTextBySpell(c,1);
		}

		return true;
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

/**
 * \brief 对话框消息处理函数
 * 
 * 对话框消息处理函数
 * 
 * \param nEvent : 消息类别
 * \param nID : 触发消息控件ID
 * \param pControl : 触发消息控件指针
 * \return 消息处理返回值
 */
bool CGuiWorldConn::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
 
	char szText[MAX_PATH]={0};
	switch(nEvent)
	{
	case EVENT_CHECKBOX_CHANGED:
		{
			if (nID==1)
			{
				m_bShowOffline = GetCheckBox(1)->GetChecked();
				RefreshIsOffLine();
			}
		}
		break;
	case EVENT_DIALOG_RESIZE:
		{
			if (!m_bStartSize)
				return false;

			SIZE size = { GetWidth(),GetHeight() };
			int dx = size.cx - m_curDlgSize.cx;
			int dy = size.cy - m_curDlgSize.cy;
			float rx = size.cx / m_curDlgSize.cx;
			float ry = size.cy / m_curDlgSize.cy;
			if(dx || dy)
			{
				AddControlSize(m_pScrollTab,dx,dy);

				AddControlSize(GetListBox(TongList),dx,dy);
				AddControlLocationByAbsoluteX(GetStatic(TongStatic),getRate(GetStatic(TongStatic))*size.cx);
				AddControlLocation(GetButton(TongButton1),0,dy);
				AddControlLocation(GetButton(TongButton2),0,dy);
				
				AddControlSize(GetListBox(SeptList),dx,dy);
				AddControlLocationByAbsoluteX(GetStatic(SeptStatic),getRate(GetStatic(SeptStatic))*size.cx);
				AddControlLocation(GetButton(SeptButton1),0,dy);
				AddControlLocation(GetButton(SeptButton2),0,dy);

				AddControlSize(GetListBox(SchoolList),dx,dy);
				AddControlLocationByAbsoluteX(GetStatic(SchoolStatic),getRate(GetStatic(SchoolStatic))*size.cx);
				AddControlLocation(GetButton(SchoolButton1),0,dy);
				AddControlLocation(GetButton(SchoolButton2),0,dy);


				AddControlSize(GetListBox(FriendList),dx,dy);
				AddControlLocation(GetButton(FriendButton1),0,dy);
				AddControlLocation(GetButton(FriendButton2),0,dy);

				AddControlSize(GetListBox(NearbyList),dx,dy);
				AddControlSize(GetListBox(NearbyNpcList),dx,dy);

				m_curDlgSize = size;
			}
			return true;
		}
		break;
	case EVENT_TAB_SELECTION_CHANGED:
		{
			CGuiListBox* pListBox = GetCurrListBox();
			if (pListBox)
			{
				if (GetCurrListBox()->GetID()!=NearbyNpcList)
					RefreshPrivate(GetCurrListBox(),GetCurrPrivateType());

				m_pSelData=ResetSelectRember(GetCurrListBox(),m_szSelectRember);
				SetPopupMenu(GetCurrMenu());
			}
		}
		break;
	case EVENT_LISTBOX_SELECTION:
		{
			if ((void*)pControl==(void*)GetCurrListBox())
				m_pSelData=ResetSelectRember((CGuiListBox*)GetCurrListBox(),m_szSelectRember);
		}
		break;
	case EVENT_LISTBOX_ITEM_DBLCLK:
		{
			if ((pControl->GetID()!=NearbyNpcList)&&(pControl->GetID()!=BadUserList))
			{
				chat(m_szSelectRember);
			}
			else
			{
				if (pControl->GetID()==NearbyNpcList)
				{
					gotoNpc();
				}
			}
		}
		break;
	case EVENT_DIALOG_COMMAND:
		m_pSelData=ResetSelectRember((CGuiListBox*)GetCurrListBox(),m_szSelectRember);
		switch(nID) 
		{
		case ENEMYButton1: //加入黑名单
			{
				stRelationStatusCmd cmd;

				strncpy(cmd.name,m_szSelectRember,
					sizeof(cmd.name));
				cmd.type=RELATION_TYPE_ENEMY;
				cmd.byState=RELATION_REMOVE;

				SEND_USER_CMD(cmd);


				AddBad(m_szSelectRember);
			}
			break;
		case ENEMYButton2:  //移除
			{	
				stRelationStatusCmd cmd;

				strncpy(cmd.name,m_szSelectRember,
					sizeof(cmd.name));
				cmd.type=RELATION_TYPE_ENEMY;
				cmd.byState=RELATION_REMOVE;

				SEND_USER_CMD(cmd);
			}
			break;
		case TongButton100: //密聊
		case SeptButton100: 
		case SchoolButton100:
		case FriendButton100:
		case NearbyButton100:
		case BadUserButton100:
		case ENEMYButton100:
			{
				if (IsSelectItem())
				{
					InsertPrivateChat(m_szSelectRember);
				}
			}
			break;
		case FriendButton15:
		case SeptButton11:
		case TongButton13:
		case SchoolButton6:
		case NearbyButton10:
		case BadUserButton9:
		case ENEMYButton101:
			{
				if( IsSelectItem() )
				{
					Copy2Clipboard( m_szSelectRember );
				}
			}
			break;
		case SchoolButton3: //招收徒弟
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddSchoolMember,"请输入要招收的徒弟名字？");
			break;
		case SchoolButton4: //逐出师门
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 逐出师门吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelSchool,m_pSelData,c_szMBOKCancel);
				}

			}
			break;

		case FriendButton3: //加入好友
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddFriendMember,"请输入要加入好友的玩家名字？");
			//GetGameGuiManager()->AddInputBox("请输入要加入好友的玩家名字？",eAction_World,(void*)3);
			break;
		case FriendButton4: //送礼物
			
			break;
		case FriendButton5: //割袍断义
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要与好友 %s 割席断交吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelFriend,m_pSelData,c_szMBOKCancel);
				}
			}
			break;
		case BadUserButton1: //加入黑名单
			GetGameGuiManager()->AddInputBox("请输入要加入黑名单的玩家名字？",eAction_World,(void*)4);
			break;
		case BadUserButton2: //解除黑名单
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 从黑名单删除吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelBad,m_pSelData,c_szMBOKCancel);
				}
			}
			break;
		case FriendButton8: //留言
			writeBoard(eAction_FriendBoard);
			break;
		case FriendButton9: //详细资料
			break;
		case SeptButton6: //　加为好友
		case TongButton8: // 
			AddFriend(m_szSelectRember);
			break;
		case FriendButton11: //普通组队
		case SeptButton7:
		case TongButton9:
			{
				GetGameGuiManager()->AddTeam();
				if(GetGameGuiManager()->m_guiTeam)
					GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember,TEAM_NORMAL);
			}
		        break;
		case FriendButton12: //荣誉组队
		case SeptButton8:
		case TongButton10: 
			{
				GetGameGuiManager()->AddTeam();
				if(GetGameGuiManager()->m_guiTeam)
					GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember,TEAM_HONOR);
			}
			break;
		case FriendButton14:  // 添加师门
		case SeptButton10:
		case TongButton12:  
			{
				if (IsSelectItem())
					AddPrentice(m_szSelectRember);
			}
			break;
#ifdef _DEBUG_GROUP
		case FriendButton13: //特殊组队
		case SeptButton9:
		case TongButton11: 
			{
				GetGameGuiManager()->AddTeam();
				//if(GetGameGuiManager()->m_guiTeam)
				//	GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember,TEAM_MAX);
			}
			break;
#endif 
		case TongButton3: //赋予称号
			if (IsSelectItem())
			{
				if (getSelectType(m_szSelectRember)==eSelectRember)
				{
					if (IsTeamer(m_szSelectRember))
					{
						sprintf(szText,"请输入赋予 %s 的称号名？",m_szSelectRember);

						GetGameGuiManager()->AddInputBox( szText,eAction_TongChangeName,m_pSelData);
					}
				}
				else
				{
					GameMessageBox("帮主的称号名不能更改！");
				}
			}
			break;
		case TongButton4: //赋予权利
			{
				if (getSelectType(m_szSelectRember)==eSelectRember)
				{
					if (IsTeamer(m_szSelectRember))
					{
						stUnionMemberPower cmd;
						strncpy(cmd.name,m_szSelectRember,sizeof(cmd.name));
						cmd.byType = QUESTION_UNION_MEMBER_POWER;

						SEND_USER_CMD(cmd);
					}

				}
				else
				{
					GameMessageBox("帮主自己的权限不能更改！");
				}
			}
			break;
		case TongButton5: //开除成员
			{
				if (IsSelectItem())
				{
					if (IsTeamer(m_szSelectRember))
					{
						if ( GetTongSeptMasterNum() < 3 )
							sprintf( szText, "如果开除 %s ,那么帮会将自动解散,你确定么?", m_szSelectRember );
						else
							sprintf(szText,"真的要把 %s 开除吗？",m_szSelectRember);

						GetGameGuiManager()->AddMessageBox( szText,eAction_DelTong,m_pSelData,c_szMBOKCancel);
					}
				}

			}
			break;
		case TongButton6: //招收成员
			{
				char title[MAX_PATH];
				int maxSeptNum = GetMaxSeptNumByTongMana( m_TongInfo.dwMana );
				if ( maxSeptNum > m_TongShowList.size() )
				{
					sprintf( title, "能招收家族上限：%u / %u。请输入招收的族长名字：", m_TongShowList.size(), maxSeptNum );
				}
				else
				{
					sprintf( title, "能招收家族上限：%u / %u。已达到上限，请提升威望或开除几个家族。", m_TongShowList.size(), maxSeptNum );
				}
				GetGameGuiManager()->AddAddMemberDialog(eAction_AddTongMember, title);
				//GetGameGuiManager()->AddInputBox("请输入招收的族长名字？",eAction_World,(void*)2);
			}
			break;
		case TongButton7: //详细资料
			break;
		case SeptButton3: //开除家族成员
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 开除吗？",m_szSelectRember);
					GetGameGuiManager()->AddMessageBox( szText,eAction_DelSept,m_pSelData,c_szMBOKCancel);
				}
			}
			break;
		case SeptButton4: //招收家族成员
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddSeptMember,"请输入招收家族成员的名字？");
			//GetGameGuiManager()->AddInputBox("请输入招收家族成员的名字？",eAction_World,(void*)8);
			break;
		case SeptButton5: //详细资料

			break;

		case NearbyButton1: //加入好友
			AddFriend(m_szSelectRember);
			break;
		case NearbyButton2: //邀请加入队伍
			if ((IsSelectItem())&&(GetGameGuiManager()->m_guiTeam))
				GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember);
			break;
		case NearbyButton3: //邀请加入家族
			AddSept(m_szSelectRember);
			break;
		case NearbyButton4: //邀请加入帮会
			AddTong(m_szSelectRember);
			break;
		case NearbyButton5: //招收徒弟
			if (IsSelectItem())
				AddPrentice(m_szSelectRember);
			break;
		case NearbyButton6: //送礼
			
			break;
		case NearbyButton7: //加入黑名单
			AddBad(m_szSelectRember);
			break;
		case NearbyButton8: //交易
			if (IsSelectItem())
				AddTrade(m_szSelectRember);
			break;
		case NearbyButton9: //查看摊位
			{
				CCharacter* pChar = GetScene()->FindCharacterByName(m_szSelectRember);
				if ( pChar && pChar->bIsShopStart() )
				{
					CGuiShopDialog::OnOpenShop( pChar->GetID() );
				}
			}
			break;
		case NearbyNpcButton1:
			gotoNpc();
			break;
		}
		break;

	case EVENT_BUTTON_CLICKED:
		m_pSelData=ResetSelectRember((CGuiListBox*)GetCurrListBox(),m_szSelectRember);
		switch(nID)
		{
		case 2139:	//家族介绍
			{
				GetGameGuiManager()->AddHelp("data\\help\\family.xml");
				//GetGameGuiManager()->AddMessageBox(SEPT_BOARD_INFO);
			}
			break;
		case 3119:	//帮会介绍
			{
				GetGameGuiManager()->AddHelp("data\\help\\union.xml");
				//GetGameGuiManager()->AddMessageBox(TONG_BOARD_INFO);
			}
			break;
		case 212:	//家族聊天
			{
				return GetChatManager().OnAddFamilyChat();
			}
			break;
		case 310:	//帮会聊天
			{
				return GetChatManager().OnAddUnionChat();
			}
			break;
		case 3:  // close 
			{
				OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				if( GetGameGuiManager()->m_guiCountry)
					 GetGameGuiManager()->m_guiCountry->Close();
				if( GetGameGuiManager()->m_guiAdoptCartoonDlg )
				{
					GetGameGuiManager()->m_guiAdoptCartoonDlg->Close();
				}
			}
			break;
		case ENEMYButton2://加入自己的黑名单
			{
				if (IsSelectItem())
				{
					stRelationStatusCmd cmd;

					strncpy(cmd.name,m_szSelectRember,
						sizeof(cmd.name));
					cmd.type=RELATION_TYPE_ENEMY;
					cmd.byState=RELATION_REMOVE;

					SEND_USER_CMD(cmd);


					AddBad(m_szSelectRember);
				}
			}
			break;
		case ENEMYButton6://移除
			{
				if (IsSelectItem())
				{
					stRelationStatusCmd cmd;

					strncpy(cmd.name,m_szSelectRember,
						sizeof(cmd.name));
					cmd.type=RELATION_TYPE_ENEMY;
					cmd.byState=RELATION_REMOVE;

					SEND_USER_CMD(cmd);
				}
			}
			break;
		case FRIENDBUTTON:
			{
				OnGuiEvent(EVENT_BUTTON_CLICKED,FRIENDPAGE,NULL);

				GetButton(nID)->SetChecked(true);
			}
			break;
		case SEPTBUTTON:
			{
				m_pScrollTab->SetCurItem(SEPT);

				GetButton(nID)->SetChecked(true);
			}
			break;
		case TONGBUTTON:
			{
				m_pScrollTab->SetCurItem(TONG);

				GetButton(nID)->SetChecked(true);
			}
			break;
		case NEARYBYBUTTON:
			{
				OnGuiEvent(EVENT_BUTTON_CLICKED,NEARBYPAGE,NULL);

				GetButton(nID)->SetChecked(true);
			}
			break;
		case TEACHERBUTTON:
			{
				m_pScrollTab->SetCurItem(SCHOOL);

				GetButton(nID)->SetChecked(true);
			}
			break;
		case FRIENDPAGE:
			{
				m_pScrollTab->SetCurItem(FRIEND);
				GetButton(nID)->SetChecked(true);
			}
			break;
		case BADPAGE:
			{
				m_pScrollTab->SetCurItem(BADUSER);
				GetButton(nID)->SetChecked(true);
			}
			break;
		case ENEMYPAGE:
			{
				m_pScrollTab->SetCurItem(ENEMYUSER);
				GetButton(nID)->SetChecked(true);
			}
			break;
		case NEARBYPAGE:
			{

				m_pScrollTab->SetCurItem(NEARBY);
				GetButton(nID)->SetChecked(true);
			}
			break;
		case NEARBYNPCPAGE:
			{

				m_pScrollTab->SetCurItem(NEARBYNPC);
				GetButton(nID)->SetChecked(true);
			}
			break;
		case SchoolButton1: //师门公告
			{
				//writeBoard(eAction_SchoolBoard);
				//SendBoard(m_pSchoolEditBoard->GetText(),CHAT_TYPE_OVERMAN_AFFICHE);
			}
			break;
		case SchoolButton2:  //离开师门
			{
				GetGameGuiManager()->AddMessageBox( "是否真的要离开师门？",eAction_ExitSchool,NULL,c_szMBOKCancel);
			}
			break;
		case SchoolButton3: //招收徒弟
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddSchoolMember,"请输入要招收的徒弟名字？");
			//GetGameGuiManager()->AddInputBox("请输入要招收的徒弟名字？",eAction_World,(void*)6);
			break;
		case SchoolButton4: //逐出师门
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 逐出师门吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelSchool,m_pSelData,c_szMBOKCancel);
				}

			}
			break;
		case SchoolButtonW:  //查看师门公告
			{
				//readBoard(eAction_SchoolBoard);
			}
			break;
		/////////////////////////桃子////////////////////////
	  case TongButton6: //招收成员
			{
				char title[MAX_PATH];
				int maxSeptNum = GetMaxSeptNumByTongMana( m_TongInfo.dwMana );
				if ( maxSeptNum > m_TongShowList.size() )
				{
					sprintf( title, "能招收家族上限：%u / %u。请输入招收的族长名字：", m_TongShowList.size(), maxSeptNum );
				}
				else
				{
					sprintf( title, "能招收家族上限：%u / %u。已达到上限，请提升威望或开除几个家族。", m_TongShowList.size(), maxSeptNum );
				}
				GetGameGuiManager()->AddAddMemberDialog(eAction_AddTongMember, title);
				//GetGameGuiManager()->AddInputBox("请输入招收的族长名字？",eAction_World,(void*)2);
			} 			
			break;
		/////////////////////////桃子///////////////////////////	
		case TongButton2: //赋予权限
			{
				if (IsSelectItem())
				{
					if (getSelectType(m_szSelectRember)==eSelectRember)
					{
						if (IsTeamer(m_szSelectRember))
						{
							stUnionMemberPower cmd;
							strncpy(cmd.name,m_szSelectRember,sizeof(cmd.name));
							cmd.byType = QUESTION_UNION_MEMBER_POWER;

							SEND_USER_CMD(cmd);
						}
					}
					else
					{
						GameMessageBox("帮主自己的权限不能更改！");
					}
				}
				break;
			}
			break;
		///////////////////////桃子///////////////////////////	
		case TongButton3: //赋予称号
			if (IsSelectItem())
			{
				if (getSelectType(m_szSelectRember)==eSelectRember)
				{
					if (IsTeamer(m_szSelectRember))
					{
						sprintf(szText,"请输入赋予 %s 的称号名？",m_szSelectRember);

						GetGameGuiManager()->AddInputBox( szText,eAction_TongChangeName,m_pSelData);
					}
				}
				else
				{
					GameMessageBox("帮主的称号名不能更改！");
				}
			}
			break;
		case TongButton1: //发起公告
			{
				if ( m_pTongEditBoard )
				{
					//writeBoard(eAction_TongBoard);
					//SendBoard(m_pTongEditBoard->GetText(),CHAT_TYPE_UNION_AFFICHE);
					stNoteUnionCmd  cmd;

					char * szStr = (char*)_alloca( m_pTongEditBoard->GetMaxCharCount()*2 + 1);
					m_pTongEditBoard->GetTextByFM( szStr );

					int len = strlen( szStr );
					if ( len >= sizeof(cmd.noteBuf)  )
						len = sizeof(cmd.noteBuf)-1;
					strncpy( cmd.noteBuf, szStr, len );
					cmd.noteBuf[len] = 0;
					if ( (len > 1) &&  (cmd.noteBuf[len-1] & 0x80) )
					{
						if ( !(cmd.noteBuf[len-2] & 0x80) )
							cmd.noteBuf[len-1] = 0;
					}

					//strncpy(cmd.noteBuf,m_pTongEditBoard->GetText(),sizeof(cmd.noteBuf));
					SEND_USER_CMD(cmd);
				}				
			}
			break;
		/////////////////////////桃子///////////////////////////	
		case TongButton7:  //开除
			{
				if (IsSelectItem())
				{
					if (IsTeamer(m_szSelectRember))
					{
						if ( GetTongSeptMasterNum() < 3 )
							sprintf( szText, "如果开除 %s ,那么帮会将自动解散,你确定么?", m_szSelectRember );
						else
							sprintf(szText,"真的要把 %s 开除吗？",m_szSelectRember);

						GetGameGuiManager()->AddMessageBox( szText,eAction_DelTong,m_pSelData,c_szMBOKCancel);
					}
				}
			}
			break;
        /////////////////////////桃子///////////////////////////
		case TongButton8:  //离开帮会
			{
				if (strcmp(m_TongInfo.master,GetScene()->GetMainCharacter()->GetName())==0)
				{
					sprintf(szText,"是否真的要解散帮会？");
				}
				else
				{
					sprintf(szText,"是否真的要离开帮会？");
				}

				GetGameGuiManager()->AddMessageBox( szText,eAction_ExitTong,NULL,c_szMBOKCancel);
			}
			break;
		case TongButtonW:// 查看帮会公告
			{
				//readBoard(eAction_TongBoard);
			}
			break;

		case SeptButton1: //发起公告
			{
				if ( m_pSeptEditBoard )
				{
					//writeBoard(eAction_SeptBoard);
					//SendBoard(m_pSeptEditBoard->GetText(),CHAT_TYPE_FAMILY_AFFICHE);
					stNoteSeptCmd  cmd;

					char * szStr = (char*)_alloca( m_pSeptEditBoard->GetMaxCharCount()*2 + 1);
					m_pSeptEditBoard->GetTextByFM( szStr );

					int len = strlen( szStr );
					if ( len >= sizeof(cmd.noteBuf)  )
						len = sizeof(cmd.noteBuf)-1;
					strncpy( cmd.noteBuf, szStr, len );
					cmd.noteBuf[len] = 0;
					if ( (len > 1) &&  (cmd.noteBuf[len-1] & 0x80) )
					{
						if ( !(cmd.noteBuf[len-2] & 0x80) )
							cmd.noteBuf[len-1] = 0;
					}

					SEND_USER_CMD(cmd);
				}
			}
			break;
		/////////////////////////桃子///////////////////////////	
		case SeptButton2: //开除家族成员
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 开除吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelSept,m_pSelData,c_szMBOKCancel);
				}
			}
			break;
		/////////////////////////桃子///////////////////////////	
		case SeptButton4: //招收家族成员
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddSeptMember,"请输入招收家族成员的名字？");
			//GetGameGuiManager()->AddInputBox("请输入招收家族成员的名字？",eAction_World,(void*)8);
			break;
		case SeptButton3:  //离开家族
			{
				if (m_SeptInfo.dwMasterTempID==GetScene()->GetMainCharacter()->GetID())
				{
					sprintf(szText,"是否真的要解散家族？");
				}
				else
				{
					sprintf(szText,"是否真的要离开家族？");
				}
					

				GetGameGuiManager()->AddMessageBox( szText,eAction_ExitSept,NULL,c_szMBOKCancel);

			}
			break;
		/////////////////////////桃子///////////////////////////	
		case SeptButton9: //发起公告
			{
				stRequestNpcInfoSeptCmd  cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case SeptButtonW:// 查看家族公告
			{
				readBoard(eAction_SeptBoard);
			}
			break;
		case SeptButton8:// 改变称号
			{
				if (IsSelectItem())
				{
					if (getSelectType(m_szSelectRember)==eSelectRember)
					{
						sprintf(szText,"请输入赋予 %s 的称号名？",m_szSelectRember);

						GetGameGuiManager()->AddInputBox( szText,eAction_SeptChangeName,m_pSelData);
					}
					else
					{
						GameMessageBox("族长的称号名不能更改！");
					}
				}
			}
			break;
		case FriendButton3: //查看留言
			readBoard(eAction_FriendBoard);
			break;
		case FriendButton1:  //发起公告
			writeBoard(eAction_FriendBoard);
			break;
		case FriendButton2: //加入黑名单
			if (IsSelectItem())
			{
				stRelationStatusCmd cmd;
				strncpy(cmd.name,m_szSelectRember,
					sizeof(cmd.name));
				cmd.type=RELATION_TYPE_FRIEND;
				cmd.byState=RELATION_REMOVE;
				SEND_USER_CMD(cmd);
				AddBad(m_szSelectRember);
			}
			break;
		case FriendButton5: //加入好友
			GetGameGuiManager()->AddAddMemberDialog(eAction_AddFriendMember,"请输入要加入好友的玩家名字？");
			break;
		case FriendButton4: //送礼物
			break;
		case FriendButton6: //割袍断义
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要与好友 %s 割席断交吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelFriend,m_pSelData,c_szMBOKCancel);
				}
			}
			break;
		case FriendButton7: //邮寄
			{
				if (IsSelectItem())
				{
					CGuiSendMailDialog* pSendMailDlg = GetGameGuiManager()->AddSendMailDlg();
					if( NULL != pSendMailDlg )
					{
						pSendMailDlg->SetSendToName(m_szSelectRember);
					}
				}
			}
			break;
		case FriendButton10: //组队邀请
			{
				if (IsSelectItem())
				{
					GetGameGuiManager()->AddTeam();
					if(GetGameGuiManager()->m_guiTeam)
						GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember,TEAM_NORMAL);
				}
			}
			break;
		case BadUserButton2: //加入黑名单
			{
				GetGameGuiManager()->AddInputBox("请输入要加入黑名单的玩家名字？",eAction_World,(void*)4);
			}
			break;
		case BadUserButton6: //删除黑名单
			{
				if (IsSelectItem())
				{
					sprintf(szText,"真的要把 %s 从黑名单删除吗？",m_szSelectRember);

					GetGameGuiManager()->AddMessageBox( szText,eAction_DelBad,m_pSelData,c_szMBOKCancel);
				}
			}
			break;

		case NearbyButton2: //加入好友
			if (IsSelectItem())
				AddFriend(m_szSelectRember);
			break;
		case NearbyButton3: //交易
			if (IsSelectItem())
			{
				AddTrade(m_szSelectRember);
			}
			break;
		case NearbyButton6: //组队
			if ((IsSelectItem())&&(GetGameGuiManager()->m_guiTeam))
				GetGameGuiManager()->m_guiTeam->ToAddTeamMember(m_szSelectRember);
			break;
		case NearbyButton1: //聊天
			if (IsSelectItem())
				chat(m_szSelectRember);
			break;
		case NearbyNpcButton5://移动到NPC
			gotoNpc();
			break;
		case CARTOON_PET:		//领养宠物列表
			{
				CGuiAdoptCartoonDialog* pAdoptDlg = GetGameGuiManager()->AddAdoptCartoonPetDialog();
				if( NULL != pAdoptDlg )
				{
					pAdoptDlg->SwitchFromWorldConn();
				}
			}
			break;
		case ID_BUTTON_COUNTRY:  //国家界面
			GetGameGuiManager()->AddCountryDlg();
			if( GetGameGuiManager()->m_guiCountry)
			{
				GetGameGuiManager()->m_guiCountry->SwitchFromWorldConnDlg();
			}
			break;
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief  查看公告内容
 * 
 * 查看公告内容
 * 
 * \param action : 公告类型
 * \return 无
 */
void  CGuiWorldConn::readBoard(enumGuiActionType  action)
{
	FUNCTION_BEGIN;
	
	if (GetQueryManager()->FindListData(action,(DWORD)false))
		return;

	const stBoardShowInfo* p=getBoardShowInfo(action,false);
	stBoardInfo* pInfo=(stBoardInfo *)GetListItem(m_boardList,p->type);
	if (pInfo)
	{
		GetGameGuiManager()->AddWorldBoard(p->strCaption,p->strTitle,pInfo->strText.c_str(),
			action,false,NULL);
		pInfo->update=false;
	}

	FUNCTION_END;
}

/**
 * \brief 写公告
 * 
 * 写公告
 * 
 * \param action : 公告类型
 * \return 无
 */
void  CGuiWorldConn::writeBoard(enumGuiActionType  action) 
{
	FUNCTION_BEGIN;

	if (GetQueryManager()->FindListData(action,(DWORD)true))
		return;

	const stBoardShowInfo* p=getBoardShowInfo(action,true);
	
	
	GetGameGuiManager()->AddWorldBoard(p->strCaption,p->strTitle,"",
		action,true,NULL);
	

	FUNCTION_END;
}

/**
 * \brief 加入消息的通知
 * 
 * 加入消息的通知
 * 
 * \return 无
 */
void CGuiWorldConn::AddChater()
{
	FUNCTION_BEGIN;

	RefreshPrivate(GetCurrListBox(),GetCurrPrivateType());

	FUNCTION_END;
}

/**
 * \brief 检测是否有新的消息
 * 
 * 检测是否有新的消息
 * 
 * \param state : 社会关系类别
 * \return 无
 */
void CGuiWorldConn::RefreshCurrData(byte state)
{
	FUNCTION_BEGIN;


	switch(state) {
	case CHAT_TYPE_UNION_PRIVATE:
		{
			RefreshPrivate(GetListBox(TongList),state);
		}
		break;
	case CHAT_TYPE_FAMILY_PRIVATE:
		{
			RefreshPrivate(GetListBox(SeptList),state);
		}
		break;
	case CHAT_TYPE_OVERMAN_PRIVATE:
		{
			RefreshPrivate(GetListBox(SchoolList),state);
		}
		break;
	case CHAT_TYPE_FRIEND_PRIVATE:
		{
			RefreshPrivate(GetListBox(FriendList),state);
		}
		break;
	case CHAT_TYPE_PRIVATE:
		{
			RefreshPrivate(GetListBox(NearbyList),state);
		}
		break;
	}

	FUNCTION_END;
}

void CGuiWorldConn::RefreshIsOffLine()
{
	FUNCTION_BEGIN;

	ResetTong();

	ResetSept();

	ResetSchool();

	ResetRational();

	ResetBadUser();

	ResetEnemy();

	FUNCTION_END;
}
/**
 * \brief 初始化帮会信息
 * 
 * 初始化帮会信息
 * 
 * \return NULL
 */
void CGuiWorldConn::InitTong()
{
	FUNCTION_BEGIN;

	m_pTongEditBoard->SetStyle(0x19,false);

	memset(&m_TongInfo,0,sizeof(m_TongInfo));
	memset(&m_TongPower,0,sizeof(m_TongPower));
	ClearItemData();

	GetStatic(TongStatic)->SetText("");
	if (GetStatic(311))
		GetStatic(311)->SetText("");
	if (GetStatic(312))
		GetStatic(312)->SetText("");
	if (GetStatic(313))
		GetStatic(313)->SetText("");

	m_pTongEditBoard->SetStyle(0x19);

	RemoveAllItem(GetListBox(TongList));

	//if (strlen(m_TongInfo.unionName)<=0)
	//	m_pTongEditBoard->SetText("");

	m_pTongEditBoard->SetText(TONG_BOARD_INFO);


	FUNCTION_END;
}

/**
 * \brief 刷新帮会信息
 * 
 * 刷新帮会信息
 * 
 * \return 无
 */
void CGuiWorldConn::ResetTong()
{
	FUNCTION_BEGIN;

	GetButton(TongButton1)->SetEnabled(false);
	GetButton(TongButton2)->SetEnabled(false);
	GetButton(TongButton3)->SetEnabled(false);
	if (GetButton(TongButton4))
		GetButton(TongButton4)->SetEnabled(false);

	GetButton(TongButton6)->SetEnabled(false);
	GetButton(TongButton7)->SetEnabled(false);
	GetButton(TongButton8)->SetEnabled(false);

	WorldConnDialog_SetButtonEnable(310,false);

	CGuiListBox* pListBox=GetListBox(TongList);
	stUnionRember*  pRember = NULL;

	RemoveAllItem(pListBox);
	
	if (strlen(m_TongInfo.unionName)<=0)
		return;
	
	char szTxt[MAX_PATH]={0};

	GetStatic(TongStatic)->SetText(m_TongInfo.unionName);
	GetStatic(311)->SetText(m_TongInfo.master);
	GetStatic(312)->SetText(_itoa(m_TongInfo.dwMana,szTxt,10));
	GetStatic(313)->SetText(_itoa(m_TongInfo.dwAction,szTxt,10));

	if (isset_state(m_TongPower,NOTIFY))
	{
		GetButton(TongButton1)->SetEnabled(true);
		m_pTongEditBoard->SetStyle(0x9);
	}	
	
	if (isset_state(m_TongPower,CHANGE_ALIAS))
	{
		GetButton(TongButton3)->SetEnabled(true);
	}
	
	if (isset_state(m_TongPower,FIRE_MEMBER))
	{
		GetButton(TongButton7)->SetEnabled(true);
	}

	if (getSelectType(m_TongInfo.master)==eSelectMine)
	{
		GetButton(TongButton1)->SetEnabled(true);
		m_pTongEditBoard->SetStyle(0x9);
		GetButton(TongButton2)->SetEnabled(true);
		GetButton(TongButton3)->SetEnabled(true);
		GetButton(TongButton7)->SetEnabled(true);
		GetButton(TongButton6)->SetEnabled(true);
	}
	
	if (IsTeamer(GetScene()->GetMainCharacter()->GetName(),false))
	{
		GetButton(TongButton8)->SetEnabled(true);
		WorldConnDialog_SetButtonEnable(310,true);		
	}

	szTxt[0] = 0;

	for (int i=0;i<m_TongShowList.size();i++)
	{
		AddListSubItem(pListBox,m_TongShowList[i].szSeptName);
		
		for (int j=0;j<m_TongShowList[i].tongList.size();j++)
		{
			if ((i==0)&&(j==0))
				AddTongListItem(pListBox,m_TongShowList[i].tongList[j],COLOR_ARGB(255,255,0,120));
			else if (j==0)
				AddTongListItem(pListBox,m_TongShowList[i].tongList[j],COLOR_ARGB(255,180,30,225));
			else
				AddTongListItem(pListBox,m_TongShowList[i].tongList[j]);
		}
	}

	RefreshPrivate(pListBox,CHAT_TYPE_UNION_PRIVATE);

	FUNCTION_END;
}


/**
 * \brief 初始化家族信息
 * 
 * 初始化家族信息
 * 
 * \return 无
 */
void CGuiWorldConn::InitSept()
{
	FUNCTION_BEGIN;
	
	//m_pSeptEditBoard->SetEnabled(false);

	memset(&m_SeptInfo,0,sizeof(m_SeptInfo));
	
	m_SeptList.clear();

	GetButton(SeptButton1)->SetEnabled(false);
	GetButton(SeptButton2)->SetEnabled(false);
	GetButton(SeptButton3)->SetEnabled(false);
	GetButton(SeptButton4)->SetEnabled(false);
	
	if (GetButton(SeptButton8))
		GetButton(SeptButton8)->SetEnabled(false);

	if (GetButton(SeptButton9))
		GetButton(SeptButton9)->SetEnabled(false);

	WorldConnDialog_SetButtonEnable(212,false);	

	GetStatic(SeptStatic)->SetText("");
	if (GetStatic(207))
		GetStatic(207)->SetText("");
	if (GetStatic(208))
		GetStatic(208)->SetText("");
	if (GetStatic(209))
		GetStatic(209)->SetText("");
	

	m_pSeptEditBoard->SetStyle(0x19);

	RemoveAllItem(GetListBox(SeptList));

	//if (strlen(m_SeptInfo.septName)<=0)
		m_pSeptEditBoard->SetText(SEPT_BOARD_INFO);
		//m_pSeptEditBoard->SetTextColor(COLOR_R5G6B5(0,250,0));


	FUNCTION_END;
}

/**
 * \brief 刷新家族信息
 * 
 * 刷新家族信息
 * 
 * \return 无
 */
void CGuiWorldConn::ResetSept()
{
	FUNCTION_BEGIN;

	CGuiListBox* pListBox=GetListBox(SeptList);
	stSeptRember*  pRember = NULL;
	char name[MAX_PATH]={0};
	const int nWidth=30;

	RemoveAllItem(pListBox);

	if (strlen(m_SeptInfo.septName)<=0)
		return;

	GetStatic(SeptStatic)->SetText(m_SeptInfo.septName);
	GetStatic(207)->SetText(m_SeptInfo.master);
	GetStatic(208)->SetText(_itoa(m_SeptInfo.dwRepute,name,10));
	GetStatic(209)->SetText(_itoa(m_SeptInfo.dwLevel,name,10));

	pRember=(stSeptRember*)GetListItem(m_SeptList,m_SeptInfo.master);

	AddSeptListItem(pListBox,pRember);

	for (SEPTLIST::iterator pointer=m_SeptList.begin();pointer!=m_SeptList.end();pointer++)
	{
		pRember=&(pointer->second);
		
		if(strcmp(m_SeptInfo.master,pRember->memberName) != 0)
		{
			AddSeptListItem(pListBox,pRember);
		}
	}

	if (m_SeptInfo.dwMasterTempID==GetScene()->GetMainCharacter()->GetID())
	{
		m_pSeptEditBoard->SetStyle(0x9);
		GetButton(SeptButton1)->SetEnabled(true);
		GetButton(SeptButton2)->SetEnabled(true);
		GetButton(SeptButton4)->SetEnabled(true);
		GetButton(SeptButton8)->SetEnabled(true);
	}

	GetButton(SeptButton3)->SetEnabled(true);
	if (GetButton(SeptButton9))
		GetButton(SeptButton9)->SetEnabled(true);

	WorldConnDialog_SetButtonEnable(212,true);

	RefreshPrivate(pListBox,CHAT_TYPE_FAMILY_PRIVATE);


	FUNCTION_END;
}

/**
 * \brief 初始化地图功能NPC信息
 * 
 * 初始化地图功能NPC信息
 * 
 * \return 无
 */
void CGuiWorldConn::InitMapNpc()
{
	FUNCTION_BEGIN;

	m_NearlyNpcList.clear();

	RemoveAllItem(GetListBox(NearbyNpcList));

	GetButton(NearbyNpcButton1)->SetEnabled(false);
	GetButton(NearbyNpcButton2)->SetEnabled(false);
	GetButton(NearbyNpcButton3)->SetEnabled(false);
	GetButton(NearbyNpcButton4)->SetEnabled(false);
	GetButton(NearbyNpcButton5)->SetEnabled(false);
	GetButton(NearbyNpcButton6)->SetEnabled(false);


	FUNCTION_END;
}

/**
 * \brief 刷新地图功能NPC信息
 * 
 * 刷新地图功能NPC信息
 * 
 * \param listMapNpc : 地图功能NPC信息
 * \return 无
 */
void CGuiWorldConn::ResetMapNpc(tListMapNpc& listMapNpc)
{
	FUNCTION_BEGIN;

	char  szText[64]={0};

	InitMapNpc();

	CGuiListBox* pListBox=GetListBox(NearbyNpcList);

	if ( pListBox )
	{
		RemoveAllItem(pListBox);

		for( size_t i=0;i<listMapNpc.size();++i )
		{
			//红高粱npc颜色
			sprintf(szText,"(%d,%d)",listMapNpc[i].ptPos.x,listMapNpc[i].ptPos.y);
			AddListItem(pListBox,(char*)listMapNpc[i].szName,szText,true,0,&(listMapNpc[i]));
		}
		pListBox->Sort<true,1>();
	}

	GetButton(NearbyNpcButton5)->SetEnabled(true);

	FUNCTION_END;
}


/**
* \brief 初始化黑名单信息
* 
* 初始化黑名单信息
* 
* \return 无
*/
void CGuiWorldConn::InitEnemy()
{
	FUNCTION_BEGIN;

	m_EnemyList.clear();

	RemoveAllItem(GetListBox(ENEMYList));
	
	if (GetButton(ENEMYButton1))
		GetButton(ENEMYButton1)->SetEnabled(false);

	if (GetButton(ENEMYButton2))
		GetButton(ENEMYButton2)->SetEnabled(true);

	if (GetButton(ENEMYButton3))
		GetButton(ENEMYButton3)->SetEnabled(false);

	//soke 送礼
	if (GetButton(ENEMYButton4))
		GetButton(ENEMYButton4)->SetEnabled(false);

	if (GetButton(ENEMYButton5))
		GetButton(ENEMYButton5)->SetEnabled(false);

	if (GetButton(ENEMYButton6))
		GetButton(ENEMYButton6)->SetEnabled(true);

	if (GetButton(ENEMYButton7))
		GetButton(ENEMYButton7)->SetEnabled(false);

	if (GetButton(ENEMYButton8))
		GetButton(ENEMYButton8)->SetEnabled(false);

	FUNCTION_END;
}

/**
* \brief 刷新黑名单信息
* 
* 刷新黑名单信息
* 
* \return 无
*/
void CGuiWorldConn::ResetEnemy()
{
	FUNCTION_BEGIN;

	CGuiListBox* pListBox=GetListBox(ENEMYList);

	RemoveAllItem(pListBox);

	for (RELATIONLIST::iterator pointer=m_EnemyList.begin();pointer!=m_EnemyList.end();pointer++)
	{
		stRelation*  pRember=&(pointer->second);
		char sz[64];
		char szCountryName[MAX_NAMESIZE] = {0};
		BOOL bRes = CCountry::Instance()->GetCountryName(pRember->country,szCountryName);
		if( bRes )
		{
			_snprintf(sz,64,"%s (%s)",pRember->name,szCountryName);
		}
		else
		{
			_snprintf(sz,64,"%s",pRember->name);
		}		
		sz[63] = 0;

		AddListItem(pListBox,sz,"",(pRember->online!=0),pRember->user_level,(void*)pRember,pRember->occupation);
	}

	RefreshPrivate(pListBox,CHAT_TYPE_FRIEND_PRIVATE);


	FUNCTION_END;
}


/**
 * \brief 初始化黑名单信息
 * 
 * 初始化黑名单信息
 * 
 * \return 无
 */
void CGuiWorldConn::InitBadUser()
{
	FUNCTION_BEGIN;

	m_BadList.clear();

	RemoveAllItem(GetListBox(BadUserList));

	GetButton(BadUserButton1)->SetEnabled(false);
	GetButton(BadUserButton2)->SetEnabled(true);
	GetButton(BadUserButton3)->SetEnabled(false);
	GetButton(BadUserButton4)->SetEnabled(false);
	GetButton(BadUserButton5)->SetEnabled(false);
	GetButton(BadUserButton6)->SetEnabled(true);
	if (GetButton(BadUserButton7))
		GetButton(BadUserButton7)->SetEnabled(false);
	if (GetButton(BadUserButton8))
		GetButton(BadUserButton8)->SetEnabled(false);

	FUNCTION_END;
}

/**
 * \brief 刷新黑名单信息
 * 
 * 刷新黑名单信息
 * 
 * \return 无
 */
void CGuiWorldConn::ResetBadUser()
{
	FUNCTION_BEGIN;

	CGuiListBox* pListBox=GetListBox(BadUserList);

	RemoveAllItem(pListBox);

	for (RELATIONLIST::iterator pointer=m_BadList.begin();pointer!=m_BadList.end();pointer++)
	{
		stRelation*  pRember=&(pointer->second);
		AddListItem(pListBox,pRember->name,"",(pRember->online!=0),pRember->user_level,(void*)pRember,pRember->occupation);
	}

	RefreshPrivate(pListBox,CHAT_TYPE_FRIEND_PRIVATE);
	

	FUNCTION_END;
}

/**
 * \brief 初始化好友信息
 * 
 * 初始化好友信息
 * 
 * \return 无
 */
void CGuiWorldConn::InitRational()
{
	FUNCTION_BEGIN;

	m_RelationList.clear();
	m_ConsortList.clear();

	RemoveAllItem(GetListBox(FriendList));

	GetButton(FriendButton1)->SetEnabled(true);
	GetButton(FriendButton2)->SetEnabled(true);
	GetButton(FriendButton3)->SetEnabled(true);
	GetButton(FriendButton4)->SetEnabled(false);
	GetButton(FriendButton5)->SetEnabled(true);
	GetButton(FriendButton6)->SetEnabled(true);
	//soke 送礼跟邮寄
	if (GetButton(FriendButton7))
		GetButton(FriendButton7)->SetEnabled(false);
	if (GetButton(FriendButton10))
		GetButton(FriendButton10)->SetEnabled(true);

	FUNCTION_END;
}

/**
 * \brief 刷新好友信息
 * 
 * 刷新好友信息
 * 
 * \return 无
 */
void CGuiWorldConn::ResetRational()
{
	FUNCTION_BEGIN;

	CGuiListBox* pListBox=GetListBox(FriendList);
	
	RemoveAllItem(pListBox);

//#ifdef  _DEBUG
//	for ( int i = 0; i < 100; i++ )
//	{
//		stRelation st;
//		sprintf(st.name,"Name--%d",i);
//		//strncpy(st.name,"Name",MAX_NAMESIZE);
//		st.type = (BYTE)1;
//		st.level = (WORD)i*91;
//		st.user_level = (WORD)i;
//		st.online = (BYTE)(i < 50);
//		st.occupation = i%50;
//		st.exploit = i%50;
//		st.country = i;
//		strncpy(st.unionName,"UnionName",MAX_NAMESIZE);
//		AddFriendListItem(pListBox,&st,GetConnGrade(RELATION_TYPE_LOVE,st.level));
//	}
//#endif

	bool cShowTitle=false,fShowTitle=false;
	for (RELATIONLIST::iterator pointer=m_ConsortList.begin();pointer!=m_ConsortList.end();pointer++)
	{
		stRelation*  pRember=&(pointer->second);
		if (!cShowTitle)
		{
			AddListSubItem(pListBox,CONSORT);
			cShowTitle=true;
		}

		AddFriendListItem(pListBox,pRember,GetConnGrade(RELATION_TYPE_LOVE,pRember->level));
	}


	for (RELATIONLIST::iterator pointer=m_RelationList.begin();pointer!=m_RelationList.end();pointer++)
	{
		stRelation*  pRember=&(pointer->second);
		if (!fShowTitle)
		{
			AddListSubItem(pListBox,FRIENDS);
			fShowTitle=true;
		}
		AddFriendListItem(pListBox,pRember,GetConnGrade(RELATION_TYPE_FRIEND,pRember->level));
	}

	RefreshPrivate(pListBox,CHAT_TYPE_FRIEND_PRIVATE);


	FUNCTION_END;
}


/**
 * \brief 初始化师门信息
 * 
 * 初始化师门信息
 * 
 * \return 
 */
void CGuiWorldConn::InitSchool()
{
	FUNCTION_BEGIN;

	//m_pSchoolEditBoard->SetEnabled(false);

	memset(&m_SchoolInfo,0,sizeof(m_SchoolInfo));
	m_SchoolInfo.byStatus=SCHOOL_STATUS_NONE;

	m_TeacherList.clear();
	m_SchoolList.clear();

	RemoveAllItem(GetListBox(SchoolList));
	
	GetStatic(SchoolStatic)->SetText("");

//	GetButton(SchoolButton1)->SetEnabled(false);
	GetButton(SchoolButton2)->SetEnabled(false);
	GetButton(SchoolButton3)->SetEnabled(false);
	GetButton(SchoolButton4)->SetEnabled(false);

	m_pSchoolEditBoard->SetStyle(0x19);

	m_pSchoolEditBoard->SetText("师门介绍:\n1.达到30级可招收10级以上无师徒关系的人为徒弟。\n2.可招收徒弟的最大人数为20。\n3.师徒关系的建立保持在九屏范围内有效。\n4.游戏中不能主动执行拜师。"  );


	FUNCTION_END;
}



/**
 * \brief 刷新师门信息
 * 
 * 刷新师门信息
 * 
 * \return 
 */
void CGuiWorldConn::ResetSchool()
{
	FUNCTION_BEGIN;

	char szText[MAX_PATH]={0};

	CGuiListBox* pListBox=GetListBox(SchoolList);
	RemoveAllItem(pListBox);
	
	GetStatic(SchoolStatic)->SetText("");

	GetButton(SchoolButton2)->SetEnabled(true);
	GetButton(SchoolButton3)->SetEnabled(true);
	GetButton(SchoolButton4)->SetEnabled(true);
	
	switch(m_SchoolInfo.byStatus) {
	case SCHOOL_STATUS_TEACHER:
		{
			//GetButton(SchoolButton1)->SetEnabled(false);


			bool tShowTitle=false,hShowTitle=false,lShowTitle=false,pShowTitle=false;

			for (int i=0;i<m_TeacherList.size();i++)
			{
				for (TEACHERLLIST::iterator pointerTeacher=m_TeacherList.begin();pointerTeacher!=m_TeacherList.end();
					pointerTeacher++)
				{

					if (pointerTeacher->second.nLine==i)
					{

						switch(pointerTeacher->second.tag) {
						case TEACHER:
							if (!tShowTitle)
							{
								tShowTitle=true;
								AddListSubItem(pListBox,TEACHERMASTER);
								
							}
							break;
						case BIGBROTHER:
							continue;
							if (!hShowTitle)
							{
								hShowTitle=true;
								AddListSubItem(pListBox,BROTHERH);
								
							}
							break;
						case LITTLEBROTHER:
							continue;
							if (!lShowTitle)
							{
								lShowTitle=true;
								AddListSubItem(pListBox,BROTHERL);
								
							}
							break;
						case PRENTICE:
							if (!pShowTitle)
							{
								pShowTitle=true;
								AddListSubItem(pListBox,PRENTICEDESC);
								
							}
							break;
						}

						//AddListItem(pListBox,pointerTeacher->second.name,
						//	GetConnGrade(RELATION_TYPE_TEACHER,pointerTeacher->second.degree),
						//	pointerTeacher->second.online,pointerTeacher->second.level,
						//	(void*)&(pointerTeacher->second),pointerTeacher->second.occupation);
						AddTeacherListItem(pListBox,&(pointerTeacher->second),
							GetConnGrade(RELATION_TYPE_TEACHER,pointerTeacher->second.degree));

					}
				}
			}
			
		}
		break;
	case SCHOOL_STATUS_SCHOOL:
		{
			DWORD      tmpClassId=0;
			bool       bShowFriend=false;
			char       szMaster[MAX_NAMESIZE]={0};
			
			GetStatic(SchoolStatic)->SetText(m_SchoolInfo.schoolName);
			if (m_SchoolList.size()<=0)
			{
				return;
			}
			

			AddListSubItem(pListBox,SCHOOLMASTER);   //师 尊显示

			for (SCHOOLLIST::iterator pointer=m_SchoolList.begin();pointer!=m_SchoolList.end();pointer++)
			{
				if (tmpClassId != pointer->classID)  //换代显示
				{
					sprintf(szText,SCHOOLGRADE,pointer->classID);
					AddListSubItem(pListBox,szText);   
				}
				
				tmpClassId = pointer->classID;

				if (strlen(pointer->name)==0)
					continue;


				memset(szText,0,sizeof(szText));
				void* p = GetListItem(m_TeacherList,pointer->name);
				if (p)
				{
					sprintf(szText,GetConnGrade(RELATION_TYPE_TEACHER,((stTeacherMemberEx*)p)->degree));
				}
				AddListItem(pListBox,pointer->name,szText,(pointer->online!=0),pointer->level,(void*)&(*pointer),pointer->occupation);
				
				if (strlen(szMaster)<=0)
				{
					strncpy(szMaster,pointer->name,sizeof(szMaster));
				}

			}

			if (m_SchoolInfo.dwMasterTempID==GetScene()->GetMainCharacter()->GetID())
			{
				m_pSchoolEditBoard->SetStyle(0x9);
				GetButton(SchoolButton1)->SetEnabled(true);
			}
		}
		break;
	}

	RefreshPrivate(pListBox,CHAT_TYPE_OVERMAN_PRIVATE);

	FUNCTION_END;
}

/**
 * \brief 寻找帮会成员信息
 * 
 * 寻找帮会信息
 * 
 * \param pszName : 帮会成员名字
 * \return 帮会成员结构信息
 */
stUnionRember* CGuiWorldConn::FindTongMember( const char* pszName )
{
	return (stUnionRember*)GetListItem( m_TongList,pszName );	
}

/**
 * \brief 寻找家族成员信息
 * 
 * 寻找家族成员信息
 * 
 * \param pszName : 家族成员名字
 * \return 家族成员结构信息
 */
stSeptRember* CGuiWorldConn::FindSeptMember( const char* pszName )
{
	return (stSeptRember*)GetListItem( m_SeptList, pszName );
}

/**
 * \brief 寻找师门成员信息
 * 
 * 寻找师门成员信息
 * 
 * \param pszName : 师门成员名字
 * \return 师门成员结构信息
 */
stTeacherMemberEx* CGuiWorldConn::FindTeacherMember( const char* pszName )
{
	return (stTeacherMemberEx*)GetListItem( m_TeacherList, pszName );
}


void  CGuiWorldConn::InsertItem(stUnionRember* pItem)
{
	for (int i=0;i<m_TongShowList.size();i++)
	{
		if(strcmp(m_TongShowList[i].szSeptName,pItem->septName) == 0)
		{
			if (pItem->bySeptMaster==1)
			{
				m_TongShowList[i].tongList.insert(m_TongShowList[i].tongList.begin(),pItem);
			}
			else
			{
				m_TongShowList[i].tongList.push_back(pItem);
			}
			
			return;
		}
	}

	stTongShow st;
	strncpy(st.szSeptName,pItem->septName,sizeof(st.szSeptName));
	st.tongList.push_back(pItem);

	if(strcmp(m_TongInfo.master,pItem->memberName) == 0)
	{
		m_TongShowList.insert(m_TongShowList.begin(),st);
	}
	else
	{
		m_TongShowList.push_back(st);
	}
}
void  CGuiWorldConn::DeleteItem(stUnionRember* pItem)
{
	for (TONGSHOWLIST::iterator pter =m_TongShowList.begin();
		pter != m_TongShowList.end();pter++)
	{
		for (std::vector<stUnionRember*>::iterator pointer = pter->tongList.begin() ;
			pointer !=pter->tongList.end(); pointer++)
		{
			if (pItem==(*pointer))
			{
				pter->tongList.erase(pointer);
				break;
			}
		}

		if (pter->tongList.size()==0)
		{
			m_TongShowList.erase(pter);
			return;
		}
	}
}

void  CGuiWorldConn::ClearItemData()
{
	for (int i=0;i<m_TongShowList.size();i++)
	{
		m_TongShowList[i].tongList.clear();
	}
	
	m_TongShowList.clear();
	m_TongList.clear();
}

/**
 * \brief 帮会服务器消息处理
 * 
 * 帮会服务器消息处理
 * 
 * \param pCmd : 服务器消息
 * \param size : 服务器消息大小
 * \return 处理返回结构 ture 成功   false 失败
 */
bool  CGuiWorldConn::TongMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	char szText[MAX_PATH]={0};

	switch(pCmd->byParam)
	{
#ifdef _WYC_OPT_SER_
	case RTN_UNION_NAME_PARA:
		{
			stRtnUnionNameCmd* pCmdInfo = (stRtnUnionNameCmd*)pCmd;
			g_UserLocalInfo.m_mapUnionName.insert( int_string_Pair(pCmdInfo->dwUnionID,pCmdInfo->name) );
			//g_UserLocalInfo.curRequesetUnionId.erase(pCmdInfo->dwUnionID);
			GetScene()->SetCharacterUnionNameByID(pCmdInfo->dwUnionID,pCmdInfo->name);
		}
		break;
#endif
	case NOTE_UNION_PARA:
		{
			stNoteUnionCmd* pCmdInfo=(stNoteUnionCmd*)pCmd;
			m_pTongEditBoard->SetText(pCmdInfo->noteBuf);
		}
		break;

	case SERVER_RETURN_UNION_CREATE_FAILED:
		{
			GetGameGuiManager()->AddWorldBoard("帮会说明","这里是输入帮会说明的地方","",
					eAction_TongExplain,true,NULL);
		}
		break;

	case UNIONMEMBER_LEAVE_UNION_PARA:  //离开帮会
		{
			InitTong();
		}
		break;

	case UNION_MEMBER_POWER_PARA: //改变权限
		{
			stUnionMemberPower* pCmdInfo=(stUnionMemberPower*)pCmd;

			CGuiUnoinPower* pDlg = GetGameGuiManager()->AddUnoinPowerDialog(pCmdInfo->name);
			if (pDlg)
			{
				pDlg->SetPower(pCmdInfo->byPower);
			}
		}
		break;

	case UNION_STATUS_CHECK_PARA:	// 满足建立帮会的条件
		{
			GetGameGuiManager()->AddInputBox("请输入帮会的名字？",eAction_World,0);
		}
		break;

	case UNION_BASE_INFO_PARA:    //行会信息
		{
			stUnionBaseInfoCmd* pCmdInfo=(stUnionBaseInfoCmd*)pCmd;

			memcpy(&m_TongInfo,pCmdInfo,sizeof(stUnionBaseInfoCmd));
			m_pTongEditBoard->SetText(pCmdInfo->note);
		}
		break;

	case BROADCAST_MEMBER_INFO_PARA:  //更新行会成员状态及信息
		{
			stBroadcastMemberInfo* pCmdInfo=(stBroadcastMemberInfo*)pCmd;
			stUnionRember*  pSt=NULL;
			switch(pCmdInfo->byStatus) {
			case UNION_MEMBER_STATUS_ONLINE:  // 行会成员上线
				pSt=(stUnionRember*)GetListItem(m_TongList,pCmdInfo->name);
				if (pSt)
				{
					pSt->byOnline=1;
					pSt->level=pCmdInfo->level;
					pSt->exploit=pCmdInfo->exploit;
				}
				break;
			case UNION_MEMBER_STATUS_OFFLINE:  // 行会成员下线
				pSt=(stUnionRember*)GetListItem(m_TongList,pCmdInfo->name);
				if (pSt)
				{
					pSt->byOnline=0;
					pSt->level=pCmdInfo->level;
				}
				break;
			case UNION_MEMBER_STATUS_FIRE:    // 行会成员被开除
				pSt=(stUnionRember*)GetListItem(m_TongList,pCmdInfo->name);
				if (pSt)
				{
					DeleteItem(pSt);
				}
				DeleteListItem(m_TongList,pCmdInfo->name);
				break;
			case UNION_MEMBER_STATUS_NEWMEMBER: // 新增加行会成员
				{
					stUnionRember  st;
					strncpy(st.memberName,pCmdInfo->name,sizeof(st.memberName));
					strncpy(st.aliasname,pCmdInfo->aliasname,sizeof(st.aliasname));
					strncpy(st.septName,pCmdInfo->septName,sizeof(st.septName));
					st.occupation=pCmdInfo->wdOccupation;
					st.level=pCmdInfo->level;
					st.byOnline=1;
					st.exploit=pCmdInfo->exploit;
					st.bySeptMaster=pCmdInfo->bySeptMaster;

					pSt=(stUnionRember*)GetListItem(m_TongList,st.memberName);
					if (pSt)
					{
						break;
					}

					InsertListItem(&st,m_TongList,st.memberName);

					pSt=(stUnionRember*)GetListItem(m_TongList,st.memberName);
					if (pSt)
					{
						InsertItem(pSt);
					}
				}
				break;
			case UNION_MEMBER_STATUS_ALIASCHANGE: // 成员别名改变
				pSt=(stUnionRember*)GetListItem(m_TongList,pCmdInfo->name);
				if (pSt)
					strncpy(pSt->aliasname,pCmdInfo->aliasname,sizeof(pSt->aliasname));
				break;
			}
		}
		break;

	case ALL_UNION_MEMBER_PARA:   //成员列表
		{
			stAllUnionMemberCmd* pCmdInfo=(stAllUnionMemberCmd*)pCmd;
			ClearItemData();
			for (int i=0;i<pCmdInfo->size;i++)
			{
				InsertListItem(&(pCmdInfo->memberList[i]),m_TongList,pCmdInfo->memberList[i].memberName);
			}
			
			stUnionRember* pMaster = (stUnionRember*)GetListItem(m_TongList,m_TongInfo.master);
			if (pMaster)
				InsertItem(pMaster);

			for (TONGLIST::iterator pointer=m_TongList.begin();pointer!=m_TongList.end();pointer++)
			{
				stUnionRember* pRember=&(pointer->second);
				
				if ((pRember!=NULL)&&(pRember!=pMaster))
					InsertItem(pRember);
			}			
		}
		break;

	case ADD_MEMBER_TO_UNION_PARA: //邀请
		{
			stAddMemberToUnionCmd* pCmdInfo=(stAddMemberToUnionCmd*)pCmd;

			sprintf(szText,"[%s] 邀请你加入 [%s] 帮会，接受邀请吗？",pCmdInfo->memberName,
				pCmdInfo->unionName);
			if (!GetQueryManager()->FindListData(eAction_Tong,pCmdInfo->memberID))
				GetGameGuiManager()->AddMessageBox(szText,eAction_Tong,(void*)pCmdInfo->memberID,c_szMBOKCancel);
		}
		break;

	case RETURN_UNION_MEMBER_INFO_PARA: //权限改变
		{
			stReturnUnionMemberInfoCmd* pCmdInfo=(stReturnUnionMemberInfoCmd*)pCmd;

			memcpy(m_TongPower,pCmdInfo->byPower,sizeof(m_TongPower));

			

			
			//if (isset_state(m_TongPower,MASTER))
			/*if (getSelectType(m_TongInfo.master)==eSelectMine)
			{
				GetButton(TongButton2)->SetEnabled(true);
			}
			else
			{
				GetButton(TongButton2)->SetEnabled(false);
			}*/
			
			
		}
		break;

	case VOTELIST_UNION_PARA: // 帮会投票列表
		{
			RefreshAllTong((stVoteListUnionCmd*)pCmd);
		}
		break;

	case VOTE_UNION_PARA: // 帮会投票消息
		{
			BallotTong((stVoteUnionCmd*)pCmd);
		}
		break;
	
	case LIST_UNION_PARA: // 本国帮会列表消息
		{
			stListUnionCmd* pCmdInfo = (stListUnionCmd*)pCmd;
			if (GetGameGuiManager()->m_guiApplyConn)
			{
				if (pCmdInfo->flag==0)
					GetGameGuiManager()->m_guiApplyConn->m_TongList.clear();

				for (int i=0;i<pCmdInfo->dwSize;i++)
				{
					stApplyConn st;

					strncpy(st.szApplyConn,pCmdInfo->data[i].unionName,sizeof(st.szApplyConn));
					strncpy(st.szName,pCmdInfo->data[i].master,sizeof(st.szName));

					GetGameGuiManager()->m_guiApplyConn->m_TongList.push_back(st);
				}

				GetGameGuiManager()->m_guiApplyConn->RefreshListBox();
			}
		}
		break;
	case REQUEST_JOIN_UNION_PARA:  //玩家请求加入帮会
		{
			switch(((stRequestJoinUnionCmd*)pCmd)->status) {
			case REQUEST_JOIN_OK:
				{
					static char szName[MAX_NAMESIZE]={0};

					strncpy(szName,((stRequestJoinUnionCmd*)pCmd)->name,MAX_NAMESIZE);

					sprintf(szText,"%s 申请要加入你的帮会，接受申请吗？",szName);

					if (!GetQueryManager()->FindListData(eAction_TongUser,(DWORD)szName))
					{
						GetGameGuiManager()->AddMessageBox(szText,eAction_TongUser,(void*)szName,c_szMBOKCancel);
					}
					else
					{
						stRequestJoinUnionCmd cmd;
						strncpy(cmd.name,szName,MAX_NAMESIZE);
						cmd.status = REQUEST_JOIN_TIMEOUT;
						SEND_USER_CMD(cmd); 
					}
				}
				break;
			case REQUEST_JOIN_CANCEL:
				sprintf(szText,"%s 拒绝你加入他的帮会！",((stRequestJoinUnionCmd*)pCmd)->name);
				GameMessageBox(szText);
				break;

			case REQUEST_JOIN_TIMEOUT:
				sprintf(szText,"%s 当前不在线或正在忙无法应答你的请求！",((stRequestJoinUnionCmd*)pCmd)->name);
				GameMessageBox(szText);
				break;
			}
				

			
		}
		break;
	}


	switch(pCmd->byParam)
	{
	case BROADCAST_MEMBER_INFO_PARA:  //更新行会成员状态及信息
	case ALL_UNION_MEMBER_PARA:   //成员列表
	case RETURN_UNION_MEMBER_INFO_PARA:
		{
			ResetTong();
		}
		break;
	}


	switch(pCmd->byParam)
	{
	case RTN_UNION_SORT_PARA:
		{

			CCountry::Instance()->SetUnionSort((const stRtnUnionSort *)pCmd);
			const stRtnUnionSort * _cmd = (const stRtnUnionSort *)pCmd;
	
			if( GetGameGuiManager()->m_guiNpcDialog)
			{
				if( _cmd->byType == COUNTRY_UNION_SORT)
					GetGameGuiManager()->m_guiNpcDialog->ShowUnionNativeSort();
				else if( _cmd->byType == WORLD_UNION_SORT)
					GetGameGuiManager()->m_guiNpcDialog->ShowUnionWorldSort();

			}


		}
		break;
	}
	return true;

	FUNCTION_END;
}


/**
 * \brief 家族服务器消息处理
 * 
 * 家族服务器消息处理
 * 
 * \param pCmd : 服务器消息
 * \param size : 服务器消息大小
 * \return 处理返回结构 ture 成功   false 失败
 */
bool  CGuiWorldConn::SeptMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	char szText[MAX_PATH]={0};

	switch(pCmd->byParam)
	{
#ifdef _WYC_OPT_SER_
	case RTN_SEPT_NAME_PARA:
		{
			stRtnSeptNameCmd* pCmdInfo=(stRtnSeptNameCmd*)pCmd;
			g_UserLocalInfo.m_mapSpetName.insert( int_string_Pair(pCmdInfo->dwSeptID,pCmdInfo->name) );
			//g_UserLocalInfo.curRequesetSpetId.erase(pCmdInfo->dwSeptID);
			GetScene()->SetCharacterSeptNameByID(pCmdInfo->dwSeptID,pCmdInfo->name);
		}
		break;
#endif
	case NOTE_SEPT_PARA:
		{
			stNoteSeptCmd* pCmdInfo=(stNoteSeptCmd*)pCmd;
			m_pSeptEditBoard->SetText(pCmdInfo->noteBuf);
		}
		break;

	case SERVER_RETURN_SEPT_CREATE_FAILED:
		{
			GetGameGuiManager()->AddWorldBoard("家族说明","这里是输入家族说明的地方","",
				eAction_SeptExplain,true,NULL);
		}
		break;

	case SEPTMEMBER_LEAVE_SEPT_PARA:  //离开家族
		{
			InitSept();
		}
		break;

	case SEPT_STATUS_CHECK_PARA:	// 满足建立家族的条件
		{
			GetGameGuiManager()->AddInputBox("请输入家族的名字？",eAction_World,(void*)9);
		}
		break;

	case SEPT_BASE_INFO_PARA:    //家族信息
		{
			stSeptBaseInfoCmd* pCmdInfo=(stSeptBaseInfoCmd*)pCmd;

			memcpy(&m_SeptInfo,pCmdInfo,sizeof(stSeptBaseInfoCmd));
			m_pSeptEditBoard->SetText(pCmdInfo->note);
		}
		break;

	case BROADCAST_SEPT_MEMBER_INFO_PARA:  //更新家族成员状态及信息
		{
			stBroadcastSeptMemberInfo* pCmdInfo=(stBroadcastSeptMemberInfo*)pCmd;
			stSeptRember*  pSt=NULL;
			switch(pCmdInfo->byStatus) {
			case SEPT_MEMBER_STATUS_ONLINE:  // 家族成员上线
				pSt=(stSeptRember*)GetListItem(m_SeptList,pCmdInfo->name);
				if (pSt)
				{
					pSt->grace=pCmdInfo->grace;
					pSt->exploit=pCmdInfo->exploit;
					pSt->byOnline=1;
					pSt->level=pCmdInfo->level;
				}
				break;
			case SEPT_MEMBER_STATUS_OFFLINE:  // 家族成员下线
				pSt=(stSeptRember*)GetListItem(m_SeptList,pCmdInfo->name);
				if (pSt)
				{
					pSt->byOnline=0;
					pSt->level=pCmdInfo->level;
				}
				break;
			case SEPT_MEMBER_STATUS_FIRE:    // 家族成员被开除
				DeleteListItem(m_SeptList,pCmdInfo->name);
				break;
			case SEPT_MEMBER_STATUS_NEWMEMBER: // 新增加家族成员
				{
					stSeptRember  st;
					strncpy(st.memberName,pCmdInfo->name,sizeof(st.memberName));
					strncpy(st.memberAliasName,pCmdInfo->aliasName,sizeof(st.memberAliasName));
					st.grace=pCmdInfo->grace;
					st.exploit=pCmdInfo->exploit;
					st.occupation=pCmdInfo->wdOccupation;
					st.level=pCmdInfo->level;
					st.byOnline=1;
		
					InsertListItem(&st,m_SeptList,st.memberName);
				}
				break;
			case SEPT_MEMBER_STATUS_ALIASCHANGE:
				pSt=(stSeptRember*)GetListItem(m_SeptList,pCmdInfo->name);
				if (pSt)
				{
					strncpy(pSt->memberAliasName,pCmdInfo->aliasName,sizeof(pSt->memberAliasName));
				}
				break;
			}
		}
		break;

	case ALL_SEPT_MEMBER_PARA:   //成员列表
		{
			stAllSeptMemberCmd* pCmdInfo=(stAllSeptMemberCmd*)pCmd;
			m_SeptList.clear();
			for (int i=0;i<pCmdInfo->size;i++)
			{
				InsertListItem(&(pCmdInfo->memberList[i]),m_SeptList,pCmdInfo->memberList[i].memberName);
			}
		}
		break;

	case ADD_MEMBER_TO_SEPT_PARA: //邀请
		{
			stAddMemberToSeptCmd* pCmdInfo=(stAddMemberToSeptCmd*)pCmd;

			sprintf(szText,"[%s] 邀请你加入 [%s] 家族，接受邀请吗？",pCmdInfo->memberName,
				pCmdInfo->septName);
			if (!GetQueryManager()->FindListData(eAction_Sept,pCmdInfo->memberID))
				GetGameGuiManager()->AddMessageBox(szText,eAction_Sept,(void*)pCmdInfo->memberID,c_szMBOKCancel);
		}
		break;

	case VOTELIST_SEPT_PARA: // 家族投票列表
		{
			RefreshAllSept((stVoteListSeptCmd*)pCmd);
		}
		break;

	case VOTE_SEPT_PARA: // 家族投票消息
		{
			BallotSept((stVoteSeptCmd*)pCmd);
		}
		break;

	case LIST_SEPT_PARA: // 本国帮会列表消息
		{
			stListSeptCmd* pCmdInfo = (stListSeptCmd*)pCmd;
			if (GetGameGuiManager()->m_guiApplyConn)
			{
				if (pCmdInfo->flag==0)
					GetGameGuiManager()->m_guiApplyConn->m_SeptList.clear();

				for (int i=0;i<pCmdInfo->dwSize;i++)
				{
					stApplyConn st;

					strncpy(st.szApplyConn,pCmdInfo->data[i].septName,sizeof(st.szApplyConn));
					strncpy(st.szName,pCmdInfo->data[i].master,sizeof(st.szName));

					GetGameGuiManager()->m_guiApplyConn->m_SeptList.push_back(st);
				}

				GetGameGuiManager()->m_guiApplyConn->RefreshListBox();
			}
		}
		break;

	case REQUEST_JOIN_SEPT_PARA:  //玩家请求加入家族
		{
	
			switch(((stRequestJoinSeptCmd*)pCmd)->status) {
			case REQUEST_JOIN_OK:
				{
					static char szName[MAX_NAMESIZE]={0};

					strncpy(szName,((stRequestJoinSeptCmd*)pCmd)->name,MAX_NAMESIZE);

					sprintf(szText,"%s 申请要加入你的家族，接受申请吗？",szName);

					if (!GetQueryManager()->FindListData(eAction_SeptUser,(DWORD)szName))
					{
						GetGameGuiManager()->AddMessageBox(szText,eAction_SeptUser,(void*)szName,c_szMBOKCancel);
					}
					else
					{
						stRequestJoinSeptCmd cmd;
						strncpy(cmd.name,szName,MAX_NAMESIZE);
						cmd.status = REQUEST_JOIN_TIMEOUT;
						SEND_USER_CMD(cmd); 
					}
				}
				break;
			case REQUEST_JOIN_CANCEL:
				sprintf(szText,"%s 拒绝你加入他的家族！",((stRequestJoinSeptCmd*)pCmd)->name);
				GameMessageBox(szText);
				break;

			case REQUEST_JOIN_TIMEOUT:
				sprintf(szText,"%s 当前不在线或正在忙无法应答你的请求！",((stRequestJoinSeptCmd*)pCmd)->name);
				GameMessageBox(szText);
				break;
			}
		}
		break;
	}


	switch(pCmd->byParam)		//需要更新到界面的...
	{
	case BROADCAST_SEPT_MEMBER_INFO_PARA:  //更新家族成员状态及信息
	case ALL_SEPT_MEMBER_PARA:   //成员列表
		{
			ResetSept();
		}
		break;
	}

	switch(pCmd->byParam)
	{
	case RTN_SEPT_SORT_PARA:
		{
			CCountry::Instance()->SetSeptSort((const stRtnSeptSort *)pCmd);

			const stRtnSeptSort * _cmd = (const stRtnSeptSort *)pCmd;
			if( GetGameGuiManager()->m_guiNpcDialog)
			{
				if( _cmd->byType == COUNTRY_SEPT_SORT)
					GetGameGuiManager()->m_guiNpcDialog->ShowSeptNativeSort();
				else if( _cmd->byType == WORLD_SEPT_SORT)
					GetGameGuiManager()->m_guiNpcDialog->ShowSeptWorldSort();

			}

		}
		break;
	}
	return true;

	FUNCTION_END;
}


/**
* \brief 好友服务器消息处理
* 
* 好友服务器消息处理
* 
* \param pCmd : 服务器消息
* \param size : 服务器消息大小
* \return 处理返回结构 ture 成功   false 失败
*/
bool CGuiWorldConn::RationalMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	
	char szText[MAX_PATH]={0};

	switch(pCmd->byParam)
	{
		case RET_USER_SORT_INFO_PARA:// 排行榜
		{
			if( ::GetGameGuiManager()->m_guiTopDialog != NULL )
			{
				if( ::GetGameGuiManager()->m_guiTopDialog->GetListBox( 12 ) != NULL )
				{
					::GetGameGuiManager()->m_guiTopDialog->GetListBox( 12 )->RemoveAllItems();

					stRetUserSortInfoCmd* pCmdInfo=(stRetUserSortInfoCmd*)pCmd;
					std::string s = "";
					std::string s2 = "                                                                          ";

					for (int i=0;i<pCmdInfo->size;i++)
					{
						char c[ 999 ] = "";

						sprintf( c, "%d", i + 1 );

						s = c + s2.substr( 0, 6 - strlen( c ) );

						s += pCmdInfo->data[ i ].userName + s2.substr( 0, 22 - strlen( pCmdInfo->data[ i ].userName ) );

						sprintf( c, "%d", pCmdInfo->data[ i ].level );

						s += c + s2.substr( 0,6 - strlen( c ) );

						::GetGameGuiManager()->m_guiTopDialog->GetListBox( 12 )->AddItem( s.c_str(), 0 );
					}
				}
			}

			break;
		}
	case RELATION_SEND_RELATIONLIST_PARA:  //更新社会关系列表
		{
			stSendRelationListCmd* pCmdInfo=(stSendRelationListCmd*)pCmd;
			m_RelationList.clear();
			for (int i=0;i<pCmdInfo->size;i++)
			{
				InsertListItem(&(pCmdInfo->member[i]),GetList(pCmdInfo->member[i].type),pCmdInfo->member[i].name);
			}
		}
		break;

	case RELATION_STATUS_PARA:  // 关系状态变更，以及邀请交互
		{
			stRelationStatusCmd* pCmdInfo=(stRelationStatusCmd*)pCmd;
			stRelation* pSt=NULL;
			switch(pCmdInfo->byState) {
			case RELATION_QUESTION:  // 发出邀请某人的询问
				{
					sprintf(szText,"[%s] 想加你为好友，接受邀请吗？",pCmdInfo->name);
					if (!GetQueryManager()->FindListData(eAction_Friend,pCmdInfo->userid))
						GetGameGuiManager()->AddMessageBox(szText,eAction_Friend,(void*)pCmdInfo->userid,c_szMBOKCancel);
				}
				break;
			case RELATION_ONLINE:  // 在线状态
				pSt=GetRationalPlayer(pCmdInfo->name);
				if (pSt)
				{
					if (pSt->type==RELATION_TYPE_FRIEND)
					{
						PlayUISound(10101);
					}
					pSt->online=1;
					pSt->occupation=pCmdInfo->occupation;
					pSt->user_level=pCmdInfo->user_level;
					pSt->country = pCmdInfo->country;
					pSt->exploit = pCmdInfo->exploit;
					strncpy(pSt->unionName,pCmdInfo->unionName,sizeof(pSt->unionName));
				}
				break;
			case RELATION_OFFLINE:  // 离线状态
				pSt=GetRationalPlayer(pCmdInfo->name);
				if (pSt)
				{
					if (pSt->type==RELATION_TYPE_FRIEND)
					{
						PlayUISound(10115);
					}
					pSt->online=0;
					pSt->user_level=pCmdInfo->user_level;
				}
				break;
			case RELATION_REMOVE:  // 删除状态
				DeleteRationalPlayer(pCmdInfo->name);
				break;
			case RELATION_ADD:    // 添加
				{
					stRelation  st;
					strncpy(st.name,pCmdInfo->name,sizeof(st.name));
					st.level=pCmdInfo->level;
					st.type=pCmdInfo->type;
					st.user_level=pCmdInfo->user_level;
					st.occupation=pCmdInfo->occupation;
					st.online=1;
					st.country = pCmdInfo->country;
					st.exploit = pCmdInfo->exploit;
					strncpy(st.unionName,pCmdInfo->unionName,sizeof(st.unionName));
					AddRationalPlayer(&st);
				}
				break;
			case RELATION_TYPECHANGE: // 类型改变
				{
					stRelation  st;
					pSt=GetRationalPlayer(pCmdInfo->name);
					if (pSt)
					{
						pSt->type=pCmdInfo->type;
						pSt->level=pCmdInfo->level;
						pSt->user_level=pCmdInfo->user_level;
						memcpy(&st,pSt,sizeof(stRelation));

						DeleteRationalPlayer(pCmdInfo->name);
						AddRationalPlayer(&st);
					}
				}
				break;
			}
		}
		break;

	case MARRY_STATUS_CHECK_PARA: //结婚
		{
			stMarryStatusCheckCmd cmd;
			stMarryStatusCheckCmd* pCmdInfo=(stMarryStatusCheckCmd*)pCmd;
			switch(pCmdInfo->byStep) {
			case MARRY_AHEAD_CHECK:
				cmd.byStep=MARRY_ITEM_CHECK;
				SEND_USER_CMD(cmd);
				break;
			case MARRY_AHEAD_CHECK_FAIL:
				GameMessageBox("不符合结婚要求！");
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			case MARRY_ITEM_CHECK:
				/*cmd.byStep=MARRY_PAY_MONEY;
				SEND_USER_CMD(cmd);*/
				GetGameGuiManager()->AddSelectMarryTypeDlg();
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			case MARRY_NO_ITEM:
				GetGameGuiManager()->AddMessageBox("缺少结婚道具，进行结婚任务获取道具吗？",eAction_MarryTask,(void*)0,c_szMBOKCancel);
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			case MARRY_ANSWER:
				GetGameGuiManager()->AddMessageBox( "同意结婚吗？", eAction_ConfirmMarriage, NULL, c_szMBOKCancel );
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			case MARRY_PAY_MONEY:
				GameMessageBox("结婚成功！");
				PlayUISound(80000); //soke 结婚成功的声音
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			case MARRY_NO_MONEY:
				GameMessageBox("结婚所需要的银子不够！");
				GetGameGuiManager()->OnNpcActionEnd();
				break;
			}

		}
		break;
	}


	switch(pCmd->byParam)
	{
	case RELATION_SEND_RELATIONLIST_PARA: 
	case RELATION_STATUS_PARA:
		{
			ResetRational();
			ResetBadUser();
			ResetEnemy();
		}
		break;
	}
	return true;

	FUNCTION_END;
}

/**
* \brief 师门服务器消息处理
* 
* 师门服务器消息处理
* 
* \param pCmd : 服务器消息
* \param size : 服务器消息大小
* \return 处理返回结构 ture 成功   false 失败
*/
bool  CGuiWorldConn::SchoolMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	
	char szText[MAX_PATH]={0};

	switch(pCmd->byParam)
	{
	case SCHOOL_BULLETIN__PARA:  //公告信息
		{
		
		}
		break;

	case SCHOOL_STATUS_CHECK_PARA:  //满足建立门派条件
		{
			//GetGameGuiManager()->AddInputBox("请输入门派的名字？",eAction_World,(void*)5);
		}
		break;

	case MEMBER_STATUS_SCHOOL_PARA: //初始化通知
		{
			stMemberStatusSchool* pCmdInfo=(stMemberStatusSchool*)pCmd;
			InitSchool();
			m_SchoolInfo.byStatus=pCmdInfo->byStatus;
			strncpy(m_SchoolInfo.schoolName,pCmdInfo->schoolName,sizeof(m_SchoolInfo.schoolName));
		}
		break;

	case SENDMEMBERINFO_SCHOOL_PARA: //师门信息列表
		{
			stSendMemberInfoCmd* pCmdInfo=(stSendMemberInfoCmd*)pCmd;
			m_TeacherList.clear();
			for (int i=0;i<pCmdInfo->size;i++)
			{
				stTeacherMemberEx stEx;
				memcpy(&stEx,&(pCmdInfo->list[i]),sizeof(stTeacherMemberEx));
				stEx.nLine=i;
				InsertListItem(&stEx,m_TeacherList,pCmdInfo->list[i].name);
			}
			ResetSchool();
		}
		break;

	case SENDLIST_SCHOOL_PARA: //门派信息列表
		{
			stSendListSchoolCmd* pCmdInfo=(stSendListSchoolCmd*)pCmd;
			switch(pCmdInfo->byPackageTag) {
			case SCHOOL_PACKAGE_HEAD:
			case (SCHOOL_PACKAGE_HEAD|SCHOOL_PACKAGE_TAIL):
				m_SchoolList.clear();
				m_SchoolInfo.dwMasterTempID=pCmdInfo->dwMasterTempID;
				break;
			}

			for (int i=0;i<pCmdInfo->size;i++)
			{
				InsertSchoolList(&(pCmdInfo->list[i]));
			}

			switch(pCmdInfo->byPackageTag) {
			case SCHOOL_PACKAGE_TAIL:
			case (SCHOOL_PACKAGE_HEAD|SCHOOL_PACKAGE_TAIL):
				ResetSchool();
				break;
			}

		}
		break;

	case FIRE_MEMBER_FROM_SCHOOL_PARA: //将指定的人开除出师门或者门派
		{
			stFireMemberFromSchoolCmd* pCmdInfo=(stFireMemberFromSchoolCmd*)pCmd;
			DeleteListItem(m_TeacherList,pCmdInfo->memberName);
			if (m_SchoolInfo.byStatus==SCHOOL_STATUS_SCHOOL)
			{
				SCHOOLLIST::iterator pointer=FindSchoolByName(pCmdInfo->memberName);

				if (pointer!=m_SchoolList.end())
				{
					m_SchoolList.erase(pointer);
				}
			}
			ResetSchool();
		}
		break;

	case SCHOOL_MEMBER_STATUS_PARA:
		{
			stSchoolMemberStatusCmd* pCmdInfo=(stSchoolMemberStatusCmd*)pCmd;

			stTeacherMemberEx* pTmember=(stTeacherMemberEx*)GetListItem(m_TeacherList,pCmdInfo->name);
			if (pTmember)
			{
				pTmember->online=pCmdInfo->byState;
				pTmember->country=pCmdInfo->country;
				strncpy(pTmember->septName,pCmdInfo->septName ,sizeof(pTmember->septName));
				strncpy(pTmember->unionName,pCmdInfo->unionName ,sizeof(pTmember->unionName));

			}

			if (m_SchoolInfo.byStatus==SCHOOL_STATUS_SCHOOL)
			{
				SCHOOLLIST::iterator pointer=FindSchoolByName(pCmdInfo->name);

				if (pointer!=m_SchoolList.end())
				{
					pointer->online=pCmdInfo->byState;
					pointer->level=pCmdInfo->level;
				}
			}
			ResetSchool();
		}
		break;

	case ADD_MEMBER_TO_SCHOOL_PARA:
		{
			stAddMemberToSchoolCmd* pCmdInfo=(stAddMemberToSchoolCmd*)pCmd;
			stSchool st;
			switch(pCmdInfo->byState) {
			case TEACHER_ANSWER_YES:
				{
					if (m_SchoolInfo.byStatus==SCHOOL_STATUS_SCHOOL)
					{
						strncpy(st.name,pCmdInfo->memberName,sizeof(st.name));
						st.online=1;
						st.occupation=pCmdInfo->wdOccupation;
						st.level=pCmdInfo->level;
						SCHOOLLIST::iterator pointer=FindSchoolByName(pCmdInfo->degree,pCmdInfo->prename);
						if (pointer!=m_SchoolList.end())
						{
							if(strcmp(pointer->name,pCmdInfo->prename) == 0)
							{
								st.classID=pointer->classID;
								st.line=pointer->line;				
								InsertSchoolList(st,pointer);
							}
							else
							{
								pointer--;
								st.classID=pointer->classID;
								st.line=pointer->line+1;
								InsertSchoolList(st,pointer);
							}
						}
						else
						{
							pointer--;
							st.classID=pCmdInfo->degree;
							if (pCmdInfo->degree==pointer->classID)
								st.line=pointer->line+1;
							else
								st.line=0;
							InsertSchoolList(st,pointer);
						}
						ResetSchool();
					}
				}
				break;
			case TEACHER_QUESTION:
				{
					sprintf(szText,"[%s] 想收你为徒弟，接受邀请吗？",pCmdInfo->memberName);
					if (!GetQueryManager()->FindListData(eAction_Teacher,pCmdInfo->memberID))
						GetGameGuiManager()->AddMessageBox(szText,eAction_Teacher,(void*)pCmdInfo->memberID,c_szMBOKCancel);
				}
				break;
			}
		}
		break;
		
		//师徒系统
	case RTN_MASTER_BOUNTY_PARA:
		{
			stRtnMasterBountyUserCmd* pCmdInfo=(stRtnMasterBountyUserCmd*)pCmd;
			TRACE("返回师傅查询\n");
			GetGameGuiManager()->AddMasterBountyDlg();
			if (GetGameGuiManager()->m_guiMasterBountyDlg)
			{
				GetGameGuiManager()->m_guiMasterBountyDlg->OnRefreshUIData(pCmdInfo);
			}			
		}
		break;

	case RTN_PRENTICE_BOUNTY_PARA:
		{
			stRtnPrenticeBountyUserCmd* pCmdInfo=(stRtnPrenticeBountyUserCmd*)pCmd;
			TRACE("返回徒弟查询\n");
			GetGameGuiManager()->AddPrenticeBountyDlg();
			if (GetGameGuiManager()->m_guiPrenticeBountyDlg)
			{
				GetGameGuiManager()->m_guiPrenticeBountyDlg->OnRefreshUIData(pCmdInfo);
			}		
		}
		break;
	}
	
	return true;

	FUNCTION_END;
}

/**
* \brief 社会关系服务器消息处理
* 
* 社会关系服务器消息处理
* 
* \param pCmd : 服务器消息
* \param size : 服务器消息大小
* \return 处理返回结构 ture 成功   false 失败
*/
bool CGuiWorldConn::ParseMessage(stNullUserCmd* pCmd,size_t size)
{
	FUNCTION_BEGIN;

	GetRelation().ParseRelationMessage(pCmd,size);

	switch(pCmd->byCmdType)
	{
	case UNION_USERCMD:
		{
			//获取帮会名称,做个id对应-->add by wyc 0603211038
			return TongMessage(pCmd,size);
		}
		break;

	case RELATION_USERCMD:
		{
			return RationalMessage(pCmd,size);
		}
		break;

	case SCHOOL_USERCMD:
		{
			return SchoolMessage(pCmd,size);
		}
		break;

	case SEPT_USERCMD:
		{
			return SeptMessage(pCmd,size);
		}
		break;
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 发布新的公告信息
 * 
 * 发布新的公告信息
 * 
 * \param pCmd : 公告消息
 * \return 无
 */
void CGuiWorldConn::AddBoard(stChannelChatUserCmd* pCmd)
{
	FUNCTION_BEGIN;

	char szText[512]={0};
	

	switch(pCmd->dwType) {
	case CHAT_TYPE_UNION_AFFICHE:
		m_pTongEditBoard->SetText(pCmd->pstrChat);
		break;
	case CHAT_TYPE_OVERMAN_AFFICHE:
		m_pSchoolEditBoard->SetText(pCmd->pstrChat);
		break;
	case CHAT_TYPE_FAMILY_AFFICHE:
		m_pSeptEditBoard->SetText(pCmd->pstrChat);
		break;
	case CHAT_TYPE_FRIEND_AFFICHE:
		{
			stBoardInfo* pInfo=(stBoardInfo*)GetListItem(m_boardList,pCmd->dwType);
			if (pInfo)
			{
				tm* chattime=NULL;
				long times=pCmd->dwChatTime;///1000;
				chattime=localtime((const time_t*)&times);
				sprintf(szText,TIMESTR,chattime->tm_year+1900,
					chattime->tm_mon+1,chattime->tm_mday,chattime->tm_hour,chattime->tm_min,chattime->tm_sec);
				sprintf(szText,"%s %s：%s",szText,pCmd->pstrName,pCmd->pstrChat);
				pInfo->update=true;
				std::string strChat=szText;
				pInfo->strText= strChat + pInfo->strText;
			}
		}
		break;
	}

	FUNCTION_END;
}

/**
* \brief 得到社会关系的信息列表
* 
* 得到社会关系的信息列表
* 
* \param id : 关系ID （夫妻，好友，黑名单）
* \return 得到社会关系的信息列表
*/
RELATIONLIST& CGuiWorldConn::GetList(byte id)
{
	FUNCTION_BEGIN;

	switch(id) {
	case RELATION_TYPE_LOVE:
		return m_ConsortList;
		break;
	case RELATION_TYPE_FRIEND:
		return m_RelationList;
		break;
	case RELATION_TYPE_BAD:
		return m_BadList;
		break;
	case RELATION_TYPE_ENEMY:
		return m_EnemyList;
		break;
	}
	return m_RelationList;

	FUNCTION_END;
}

/**
 * \brief 根据玩家名称得到社会关系中玩家的结构信息
 * 
 * 根据玩家名称得到社会关系中玩家的结构信息
 * 
 * \param name : 玩家名称
 * \return 根据玩家名称得到社会关系中玩家的结构信息
 */
stRelation*  CGuiWorldConn::GetRationalPlayer(char* name,byte type)
{
	FUNCTION_BEGIN;

	stRelation* pSt=NULL;
	

	if (type!=255)
	{
		pSt=(stRelation*)GetListItem(GetList(type),name);
		if (pSt)
			return pSt;
	}
	else
	{
		pSt=(stRelation*)GetListItem(m_ConsortList,name);
		if (pSt)
			return pSt;

		pSt=(stRelation*)GetListItem(m_RelationList,name);
		if (pSt)
			return pSt;

		pSt=(stRelation*)GetListItem(m_BadList,name);
		if (pSt)
			return pSt;
	
		pSt=(stRelation*)GetListItem(m_EnemyList,name);
		if (pSt)
			return pSt;
	}

	return pSt;

	FUNCTION_END;
}

/**
 * \brief 根据玩家名称删除社会关系中玩家的结构信息
 * 
 * 根据玩家名称删除社会关系中玩家的结构信息
 * 
 * \param name : 玩家名称
 * \return 无
 */
void CGuiWorldConn::DeleteRationalPlayer(char* name,byte type)
{
	FUNCTION_BEGIN;
	
	if (type!=255)
	{
		DeleteListItem(GetList(type),name);
	}
	else
	{
		DeleteListItem(m_ConsortList,name);
		DeleteListItem(m_RelationList,name);
		DeleteListItem(m_BadList,name);
		DeleteListItem(m_EnemyList,name);
	}

	

	FUNCTION_END;
}

/**
 * \brief 加入一个社会关系玩家的信息结构
 * 
 * 加入一个社会关系玩家的信息结构
 * 
 * \param pSt : 玩家的信息结构
 * \return 无
 */
void CGuiWorldConn::AddRationalPlayer(stRelation* pSt)
{
	FUNCTION_BEGIN;

	switch(pSt->type) {
	case RELATION_TYPE_LOVE:
		InsertListItem(pSt,m_ConsortList,pSt->name);
		break;
	case RELATION_TYPE_FRIEND:
		InsertListItem(pSt,m_RelationList,pSt->name);
		break;
	case RELATION_TYPE_BAD:
		InsertListItem(pSt,m_BadList,pSt->name);
		break;
	case RELATION_TYPE_ENEMY:
		InsertListItem(pSt,m_EnemyList,pSt->name);
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 根据门派代数和玩家名称来查找玩家信息结构
 * 
 * 根据门派代数和玩家名称来查找玩家信息结构
 * 
 * \param classID : 门派代数
 * \param szName : 玩家名称
 * \return 玩家信息结构
 */
SCHOOLLIST::iterator   CGuiWorldConn::FindSchoolByName(DWORD classID,char* szName)
{
	FUNCTION_BEGIN;

	static SCHOOLLIST::iterator pointer;
	bool isClass=false;
	for (pointer=m_SchoolList.begin();pointer!=m_SchoolList.end();
		pointer++)
	{
		if (pointer->classID==classID)
		{
			isClass=true;
			if(strcmp(pointer->name,szName) == 0)
				return pointer;
		}
		else
		{
			if (isClass)
			{
				return pointer;
			}
		}
	}
	return pointer;

	FUNCTION_END;
}


/**
 * \brief 根据玩家名称来查找玩家信息结构
 * 
 * 根据玩家名称来查找玩家信息结构
 * 
 * \param szName : 玩家名称
 * \return 玩家信息结构
 */
SCHOOLLIST::iterator   CGuiWorldConn::FindSchoolByName(char* szName)
{
	FUNCTION_BEGIN;

	static SCHOOLLIST::iterator pointer;
	for (pointer=m_SchoolList.begin();pointer!=m_SchoolList.end();
		pointer++)
	{
		if(strcmp(pointer->name,szName) == 0)
			return pointer;
	}
	return pointer;

	FUNCTION_END;
}


/**
 * \brief 插入一条新的师门玩家信息结构
 * 
 * 插入一条新的师门玩家信息结构
 * 
 * \param st : 师门玩家信息结构
 * \param pointer : 要插入的位置
 * \return 无
 */
void CGuiWorldConn::InsertSchoolList(stSchool& st,SCHOOLLIST::iterator pointer)
{
	FUNCTION_BEGIN;

	pointer++;
	if (pointer!=m_SchoolList.end())
	{
		m_SchoolList.insert(pointer,st);
	}
	else
	{
		m_SchoolList.push_back(st);
	}

	FUNCTION_END;
}

/**
 * \brief 在链表最后插入一条师门玩家信息结构
 * 
 * 在链表最后插入一条师门玩家信息结构
 * 
 * \param pMber : 师门玩家信息结构
 * \return 无
 */
void  CGuiWorldConn::InsertSchoolList(stSchoolMember* pMber)
{
	FUNCTION_BEGIN;

	if (pMber==NULL)
	{
		return;
	}
	stSchool st;
	strncpy(st.name,pMber->name,sizeof(st.name));
	st.online = pMber->online;
	st.occupation = pMber->occupation;
	if (m_SchoolList.size()>0)
	{
		SCHOOLLIST::iterator pointer=m_SchoolList.end();
		pointer--;
		switch(pMber->tag) {
		case SCHOOL_NEWLAYER:
			st.classID=pointer->classID+1;
			st.line=0;
			break;

		case SCHOOL_NEWLINE:
			st.classID=pointer->classID;
			st.line=pointer->line+1;
			break;

		case SCHOOL_NONE:
			st.classID=pointer->classID;
			st.line=pointer->line;
			break;
		}
	}

	m_SchoolList.push_back(st);

	FUNCTION_END;
}

/**
 * \brief 与指定的玩家私聊
 * 
 * 与指定的玩家私聊
 * 
 * \param name : 玩家名字
 * \return 无
 */
void CGuiWorldConn::chat(const char* name)
{
	FUNCTION_BEGIN;

	if (getSelectType(name)!=eSelectRember)
		return;
	
	GetChatManager().OnChat(name,m_pChannel);

	FUNCTION_END;
}

/**
 * \brief 返回当前选中项的类型
 * 
 * 返回当前选中项的类型 （自己、其他玩家、空）
 * 
 * \param name : 选中的名字
 * \return 返回当前选中项的类型（自己、其他玩家、空）
 */
CGuiWorldConn::enumSelect CGuiWorldConn::getSelectType(const char* name)
{
	FUNCTION_BEGIN;

	if (strlen(name)==0)
		return eSelectNone;
	if (strcmp(GetScene()->GetMainCharacter()->GetName(),name) == 0)
		return eSelectMine;

	return eSelectRember;

	FUNCTION_END;
}

/**
 * \brief 清空当前的列表项
 * 
 * 清空当前的列表项
 * 
 * \param pListBox : 清空的列表项控件
 * \return 无
 */
void  CGuiWorldConn::RemoveAllItem(CGuiListBox* pListBox)
{
	FUNCTION_BEGIN;

	if (!pListBox)
		return;
	
	if (pListBox!=GetListBox(NearbyNpcList))
	{
		ResetSelectRember(pListBox,s_szCurSel);
	}

	pListBox->RemoveAllItems();

	if ((void*)pListBox == (void*)GetCurrListBox())
	{
		memset(m_szSelectRember,0,sizeof(m_szSelectRember));
	}

	FUNCTION_END;
}

/**
 * \brief 得到当前的菜单指针
 * 
 * 得到当前的菜单指针
 * 
 * \return 得到当前的菜单指针
 */
CGuiMenu*  CGuiWorldConn::GetCurrMenu()
{
	FUNCTION_BEGIN;

	stTabInfo* pData=(stTabInfo*)GetListItem(m_TabList,m_pScrollTab->GetCurItem());
	if (pData)
		return pData->pMenu;

	return NULL;

	FUNCTION_END;
}

/**
 * \brief 得到当前聊天者的角色类型
 * 
 * 得到当前聊天者的角色类型
 * 
 * \return 得到当前聊天者的角色类型
 */
byte  CGuiWorldConn::GetCurrPrivateType()
{
	FUNCTION_BEGIN;

	stTabInfo* pData=(stTabInfo*)GetListItem(m_TabList,m_pScrollTab->GetCurItem());
	if (pData)
		return pData->ePrivateType;

	return 0;

	FUNCTION_END;
}

/**
 * \brief 得到当前的公告类型
 * 
 * 得到当前的公告类型
 * 
 * \return 得到当前的公告类型
 */
byte  CGuiWorldConn::GetCurrBoardType()
{
	FUNCTION_BEGIN;

	stTabInfo* pData=(stTabInfo*)GetListItem(m_TabList,m_pScrollTab->GetCurItem());
	if (pData)
		return pData->eBoardType;

	return 0;

	FUNCTION_END;
}

/**
 * \brief 得到当前列表项指针
 * 
 * 得到当前列表项指针
 * 
 * \return 得到当前列表项指针
 */
CGuiListBox*  CGuiWorldConn::GetCurrListBox()
{
	FUNCTION_BEGIN;

	if (m_pScrollTab==NULL)
		return NULL;

	stTabInfo* pData=(stTabInfo*)GetListItem(m_TabList,m_pScrollTab->GetCurItem());
	if (pData)
		return pData->pListBox;
	

	return NULL;

	FUNCTION_END;
}

/**
 * \brief 刷新当前选中的玩家或NPC
 * 
 * 刷新当前选中的玩家或NPC
 * 
 * \param pListBox : 要刷新的列表项
 * \param szBuffer : 要保存的最新的玩家或NPC的名字
 * \return 返回最新的玩家或NPC的社会关系结构指针
 */
#define  STRCOPY(pSrc,pDes) strncpy(pDes,pSrc,MAX_NAMESIZE)
void*  CGuiWorldConn::ResetSelectRember(CGuiListBox* pListBox,char* szBuffer)
{
	FUNCTION_BEGIN;

	//static int bufferLen=MAX_NAMESIZE/2;
	
	if (!pListBox||!szBuffer)
		return NULL;

	memset(szBuffer,0,sizeof(szBuffer));
	void*       pData = pListBox->GetItemData(pListBox->GetCurItem());
	if (pData)
	{
		switch(pListBox->GetID()) {
		case TongList:
			{
				stUnionRember* pRember = (stUnionRember*) pData;
				//wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pRember->memberName,
				//	bufferLen);
				//strncpy(szBuffer,pRember->memberName,MAX_NAMESIZE);
				STRCOPY(pRember->memberName,szBuffer);
			}
			break;
		case SeptList:
			{
				stSeptRember* pRember = (stSeptRember*) pData;
				//wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pRember->memberName,
				//	bufferLen);
				STRCOPY(pRember->memberName,szBuffer);
			}
			break;
		case SchoolList:
			{
				switch(m_SchoolInfo.byStatus) {
				case SCHOOL_STATUS_TEACHER:
					{
						stTeacherMember* pRember = (stTeacherMember*) pData;
						//wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pRember->name,
						//	bufferLen);
						STRCOPY(pRember->name,szBuffer);
					}
					break;
				case SCHOOL_STATUS_SCHOOL:
					{
						stSchool* pRember = (stSchool*) pData;
						/*wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pRember->name,
							bufferLen);*/
						STRCOPY(pRember->name,szBuffer);
					}
					break;
				}
			}
			break;
		case BadUserList:
		case FriendList:
		case ENEMYList:						//soke 点击了list后
			{
				stRelation* pRember = (stRelation*) pData;
				/*wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pRember->name,
					bufferLen);*/
				STRCOPY(pRember->name,szBuffer);
			}
			break;
		case NearbyList:
			{
				std::string* pStr=(std::string*)pData;
				/*wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pStr->data(),
					bufferLen);*/
				STRCOPY(pStr->c_str(),szBuffer);
			}
			break;
		case NearbyNpcList:
			{
				stMapFunctionNpc* pNpcInfo=(stMapFunctionNpc*) pData;
				/*wcsncpy((wchar_t*)szBuffer,(const wchar_t*)pNpcInfo->szName,
					bufferLen);*/
				STRCOPY(pNpcInfo->szName,szBuffer);
			}
			break;
		}

	}
	return pData;

	FUNCTION_END;
}


/**
 * \brief 刷新附近玩家数据
 * 
 * 刷新附近玩家数据
 * 
 * \return 无
 */
void CGuiWorldConn::RefreshCharacter()
{
	FUNCTION_BEGIN;
	
	CGuiMenu* pMenu = GetPopupMenu();

	if (pMenu!=NULL)
	{
		if (pMenu->IsVisible())
			return;
	}

	CGuiListBox* pListBox=GetListBox(NearbyList);
	if ( pListBox )
	{
		std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;

		GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());

		RemoveAllItem(pListBox);

		int num = getMin(maxNearNum,(int)aCharacter.size());

		for( size_t i=0;i<num;++i )
		{
			m_NearlyList[i]=aCharacter[i]->GetName();
			AddListItem(pListBox,(char*)aCharacter[i]->GetName(),"",true,0,&(m_NearlyList[i]),aCharacter[i]->GetType());
		}
		pListBox->Sort<true,1>();//按拼音顺序排序
	}


	GetButton(NearbyButton1)->SetEnabled(true);
	GetButton(NearbyButton2)->SetEnabled(true);
	GetButton(NearbyButton3)->SetEnabled(true);
	GetButton(NearbyButton4)->SetEnabled(false);
	GetButton(NearbyButton5)->SetEnabled(false);
	GetButton(NearbyButton6)->SetEnabled(true);


	FUNCTION_END;
}

/**
 * \brief 刷新附近NPC数据
 * 
 * 刷新附近NPC数据
 * 
 * \return 无
 */
//void CGuiWorldConn::RefreshNpc()
//{
//	FUNCTION_BEGIN;
//
//	char  szText[64]={0};
//	CGuiListBox* pListBox=GetListBox(NearbyNpcList);
//
//	std::vector< CNpc*, std::stack_allocator<CNpc*> > aNpc;
//
//	GetScene()->GetObjectOrderByDistance(aNpc,type2type_t<CNpc>());
//
//	RemoveAllItem(pListBox);
//
//	int num = getMin(maxNearNum,(int)aNpc.size());
//
//	for( size_t i=0;i<num;++i )
//	{
//		m_NearlyNpcList[i].pt=aNpc[i]->GetGridPos();
//		m_NearlyNpcList[i].name=aNpc[i]->GetName();
//		
//		sprintf(szText,"(%d,%d)",aNpc[i]->GetGridPos().x,aNpc[i]->GetGridPos().y);
//	}
//
//
//	FUNCTION_END;
//}

static std::string sGotoNpcName;
static void OnGotoNpcFinal(void* pParam)
{
	CNpc* pNpc = GetScene()->FindNpcByName(sGotoNpcName.c_str());
	if(pNpc)
		GetScene()->GetMainCharacter()->CallNpc(pNpc);
}
/**
 * \brief 自动走到当前选中的NPC位置
 * 
 * 自动走到当前选中的NPC位置
 * 
 * \return 无
 */
void CGuiWorldConn::gotoNpc()
{
	FUNCTION_BEGIN;

	CGuiListBox* pListBox=GetListBox(NearbyNpcList);

	stMapFunctionNpc* pData=(stMapFunctionNpc*)pListBox->GetItemData(pListBox->GetCurItem());
	if (pData)
	{
		sGotoNpcName = pData->szName;
		GetScene()->GetMainCharacter()->Goto(pData->ptPos,OnGotoNpcFinal,NULL);
	}
	else
		GameMessageBox("请先选中要移动到的NPC!");


	FUNCTION_END;
}

/**
 * \brief 刷新当前的帮会投票数据
 * 
 * 刷新当前的帮会投票数据
 * 
 * \param pCmd : 帮会投票数据消息
 * \return 无
 */
void CGuiWorldConn::RefreshAllTong(stVoteListUnionCmd* pCmd)
{
	if (GetGameGuiManager()->m_guiBallot)
	{
		if (pCmd->flag==0)
			GetGameGuiManager()->m_guiBallot->m_BallotList.clear();

		for (int i=0;i<pCmd->dwSize;i++)
		{
			stBallot st;

			strncpy(st.szBallot,pCmd->data[i].unionName,sizeof(st.szBallot));
			strncpy(st.szName,pCmd->data[i].master,sizeof(st.szName));
			st.nBallot = pCmd->data[i].wdVoteNumber;
			strncpy(st.szContext,pCmd->data[i].note,sizeof(st.szContext));

			GetGameGuiManager()->m_guiBallot->m_BallotList.push_back(st);
		}

		GetGameGuiManager()->m_guiBallot->RefreshListBox(pCmd->unionName);
	}
}

/**
 * \brief 刷新当前的家族投票数据
 * 
 * 刷新当前的家族投票数据
 * 
 * \param pCmd : 家族投票数据消息
 * \return 无
 */
void CGuiWorldConn::RefreshAllSept(stVoteListSeptCmd* pCmd)
{
	if (GetGameGuiManager()->m_guiBallot)
	{
		if (pCmd->flag==0)
			GetGameGuiManager()->m_guiBallot->m_BallotList.clear();
		
		for (int i=0;i<pCmd->dwSize;i++)
		{
			stBallot st;

			strncpy(st.szBallot,pCmd->data[i].septName,sizeof(st.szBallot));
			strncpy(st.szName,pCmd->data[i].master,sizeof(st.szName));
			st.nBallot = pCmd->data[i].wdVoteNumber;
			strncpy(st.szContext,pCmd->data[i].note,sizeof(st.szContext));

			GetGameGuiManager()->m_guiBallot->m_BallotList.push_back(st);
		}

		GetGameGuiManager()->m_guiBallot->RefreshListBox(pCmd->septName);
	}
}

/**
 * \brief 帮会投票成功处理
 * 
 * 帮会投票成功处理
 * 
 * \param pCmd : 帮会投票成功消息
 * \return 无
 */
void CGuiWorldConn::BallotTong(stVoteUnionCmd* pCmd)
{
	if (GetGameGuiManager()->m_guiBallot)
	{
		GetGameGuiManager()->m_guiBallot->Ballot(pCmd->unionName,pCmd->bySuccess);
	}
}

/**
 * \brief 家族投票成功处理
 * 
 * 家族投票成功处理
 * 
 * \param pCmd : 家族投票成功消息
 * \return 无
 */
void CGuiWorldConn::BallotSept(stVoteSeptCmd* pCmd)
{
	if (GetGameGuiManager()->m_guiBallot)
	{
		GetGameGuiManager()->m_guiBallot->Ballot(pCmd->septName,pCmd->bySuccess);
	}
}

void CGuiWorldConn::SwitchFromCountryDlg()
{
	FUNCTION_BEGIN;

	if( GetGameGuiManager()->m_guiCountry )
	{

		 if( GetGameGuiManager()->m_guiCountry->IsSeptClicked() )
		 {
			OnGuiEvent(EVENT_BUTTON_CLICKED,SEPTBUTTON,NULL);
		 }
		 else if(  GetGameGuiManager()->m_guiCountry->IsFriendClicked())
		 {
			 OnGuiEvent(EVENT_BUTTON_CLICKED,FRIENDBUTTON,NULL);
		 }
		 else if( GetGameGuiManager()->m_guiCountry->IsTongClicked())
		 {
			 OnGuiEvent(EVENT_BUTTON_CLICKED,TONGBUTTON,NULL);
		 }
		 else if(GetGameGuiManager()->m_guiCountry->IsSchoolClicked())
		 {
			 OnGuiEvent(EVENT_BUTTON_CLICKED,TEACHERBUTTON,NULL);
		 }
	}
 
	FUNCTION_END;
}

void CGuiWorldConn::SwitchFromAdoptCartoonDlg(UINT uID)
{
	FUNCTION_BEGIN;
	OnGuiEvent(EVENT_BUTTON_CLICKED,uID,NULL);
	FUNCTION_END;
}

/**
 * \brief 加入一个好友通知服务器
 * 
 * 加入一个好友通知服务器
 * 
 * \param name : 要添加好友的名字
 * \return 无
 */
void AddFriend(const char* name)
{
	FUNCTION_BEGIN;

	stRelationStatusCmd cmd;
	strncpy(cmd.name,name,sizeof(cmd.name));
	cmd.type=RELATION_TYPE_FRIEND;
	cmd.byState=RELATION_QUESTION;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 创建家族通知服务器
 * 
 * 创建家族通知服务器
 * 
 * \param name : 要创建的家族名字
 * \return 无
 */
void AddSept(const char* name)
{
	FUNCTION_BEGIN;

	stAddMemberToSeptCmd cmd;
	strncpy(cmd.memberName,name,sizeof(cmd.memberName));
	cmd.byState=SEPT_QUESTION;
	strncpy(cmd.septName,
		GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName,
		sizeof(cmd.septName));
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 加入一个黑名单通知服务器
 * 
 * 加入一个黑名单通知服务器
 * 
 * \param name : 要添加黑名单的名字
 * \return 无
 */
void AddBad(const char* name)
{
	FUNCTION_BEGIN;

	stRelationStatusCmd cmd;
	strncpy(cmd.name,name,sizeof(cmd.name));
	cmd.type=RELATION_TYPE_BAD;
	cmd.byState=RELATION_ADD;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 创建帮会通知服务器
 * 
 * 创建帮会通知服务器
 * 
 * \param name : 要创建的帮会名字
 * \return 无
 */
void AddTong(const char* name)
{
	FUNCTION_BEGIN;

	stAddMemberToUnionCmd cmd;
	strncpy(cmd.memberName,name,sizeof(cmd.memberName));
	cmd.byState=QUESTION;
	strncpy(cmd.unionName,
		GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName,
		sizeof(cmd.unionName));
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 邀请玩家加入师门
 * 
 * 邀请玩家加入师门
 * 
 * \param name : 邀请玩家的名字
 * \return 无
 */
void AddPrentice(const char* name)
{
	FUNCTION_BEGIN;

	stAddMemberToSchoolCmd cmd;
	strncpy(cmd.memberName,name,sizeof(cmd.memberName));
	cmd.byState=TEACHER_QUESTION;
	strncpy(cmd.schoolName,
		GetGameGuiManager()->m_guiWorldConn->m_SchoolInfo.schoolName,
		sizeof(cmd.schoolName));
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 是否可建立家族询问服务器
 * 
 * 是否可建立家族询问服务器
 * 
 * \return 无
 */
void RequestSept()
{
	FUNCTION_BEGIN;

	stSeptStatusCheckCmd  cmd;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 是否可结婚询问服务器
 * 
 * 是否可结婚询问服务器
 * 
 * \return 无
 */
void RequestMarry()
{
	FUNCTION_BEGIN;

	stMarryStatusCheckCmd  cmd;
	cmd.byStep=MARRY_AHEAD_CHECK;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 是否可建立帮会询问服务器
 * 
 * 是否可建立帮会询问服务器
 * 
 * \return 无
 */
void RequestTong()
{
	FUNCTION_BEGIN;

	stUnionStatusCheckCmd  cmd;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 是否可建立师门询问服务器
 * 
 * 是否可建立师门询问服务器
 * 
 * \return 无
 */
void RequestSchool()
{
	FUNCTION_BEGIN;

	stSchoolStatusCheckCmd  cmd;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

/**
 * \brief 解散家族
 * 
 * 解散家族
 * 
 * \return 无
 */
void ShutDownSept()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 解散帮会
 * 
 * 解散帮会
 * 
 * \return 无
 */
void ShutDownTong()
{
	FUNCTION_BEGIN;



	FUNCTION_END;
}

/**
 * \brief 解散师门
 * 
 * 解散师门
 * 
 * \return 无
 */
void ShutDownSchool()
{
	FUNCTION_BEGIN;

	stDestroySchoolCmd  cmd;
	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

void CGuiWorldConn::OnChannelClosed(CChatChannel* pChannel)
{
	if( GetGameGuiManager()->m_guiNearby )
	{
		CChatChannel* p = GetGameGuiManager()->m_guiNearby->GetChatChannel();
		if( pChannel == p )
		{
			GetGameGuiManager()->m_guiNearby->SetChatChannel(NULL);
		}
	}

	//
	if( GetGameGuiManager()->m_guiWorldConn )
	{
		CChatChannel* p = GetGameGuiManager()->m_guiWorldConn->GetChatChannel();
		if( pChannel == p )
		{
			GetGameGuiManager()->m_guiWorldConn->SetChatChannel(NULL);
		}
	}
}
