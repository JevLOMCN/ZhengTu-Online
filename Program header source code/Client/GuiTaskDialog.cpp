/**
 *\file		GuiTaskDialog.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-04 17:00:00
 *\brief	任务对话框
 *
 * 任务对话框：显示任务、任务描述、任务难度
 *
 */

#include "public.h"
#include "./GameGuiManager.h"
#include "guitaskdialog.h"
#include "./game_script.h"
#include "../xml_parse/XmlParseEx.h"
#include "GameScene.h"

//分支页面
static const int scTabID = 1;

///任务列表框id
static const int scMainTaskListBoxID = 2;
static const int scSwitchTaskListBoxID = 12;

//注销任务按钮
static const int scCancelMainTask = 3;
static const int scCancelSwtichTask = 13;

///任务描述说明编辑框id
static const int scMainTaskMlTextBoxID = 4;
static const int scSwitchTaskMlTextBoxID = 14;

//任务分支切换按钮
static const int scTaskMainBtnID = 5;
static const int scTaskSwitchBtnID = 15;
//任务王检索按钮
static const int scTaskKingBtnID = 20;
static const int scTaskKingMlTextBoxID = 201;

//soke 关闭按钮
static const int scTaskCloseBtnID = 100;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwTaskID : 描述
 * \param variableName : 描述
 * \return 返回值的描述
 */


/**
 * \brief 构造函数
 * 
 */
CGuiTaskDialog::CGuiTaskDialog()
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	m_dwCurShownTask = 0;

	for(int i = 0;i < 5;i++)
	{
		m_MainTaskEasyStars[i] = NULL;
		m_MainTaskHardStars[i] = NULL;
		m_SwitchTaskEasyStars[i]= NULL;
		m_SwitchTaskHardStars[i]= NULL;
	}

	FUNCTION_END;
}

/**
* \brief "关闭"消息响应函数
* 
* 退出对话框、更新管理器对应的指针
* 
* \return void
*/
void CGuiTaskDialog::OnClose()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guitaskdialog = NULL;

	m_pMainTaskList = NULL;
	m_mlMainTaskTextBox = NULL;
	for(int i = 0;i < 5;i++)
	{
		m_MainTaskEasyStars[i] = NULL;
		m_MainTaskHardStars[i] = NULL;
		m_SwitchTaskEasyStars[i]= NULL;
		m_SwitchTaskHardStars[i]= NULL;
	}

	GetGameGuiManager()->OnNpcActionEnd();


	FUNCTION_END;
}

/**
 * \brief "创建"消息响应函数
 * 
 * 初始化各个控件(ListBox，MLEditBox)，隐藏任务难度stars
 * 
 * \return void
 */
void CGuiTaskDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	//soke 任务显示小字体
	m_mlMainTaskTextBox = GetMLTextBox( scMainTaskMlTextBoxID );
	if(m_mlMainTaskTextBox) m_mlMainTaskTextBox->SetFont(0);
	m_pMainTaskList = GetListBox( scMainTaskListBoxID );
	if(m_pMainTaskList) m_pMainTaskList->SetFont(0);

	m_mlSwitchTaskTextBox = GetMLTextBox( scSwitchTaskMlTextBoxID );
	if(m_mlSwitchTaskTextBox) m_mlSwitchTaskTextBox->SetFont(0);
	m_pSwitchTaskList = GetListBox( scSwitchTaskListBoxID );
	if(m_pSwitchTaskList) m_pSwitchTaskList->SetFont(0);

	m_mlTaskKingTextBox = GetMLTextBox( scTaskKingMlTextBoxID );
	if(m_mlTaskKingTextBox)
	{
		m_mlTaskKingTextBox->SetFont(1);

		if(IsFileExist("taskking.txt"))
		{
			char * npcScript;
			FileStream file;
			file.open("taskking.txt",FileStream::Read);
			npcScript = new char[file.getSize()+1];
			file.read(npcScript,file.getSize());
			npcScript[file.getSize()] = 0;
			m_mlTaskKingTextBox->SetText( npcScript );
			delete [] npcScript;
		}
		else
		{
			m_mlTaskKingTextBox->SetText( "暂时无任务" );
		}
	}

	/*for(int i = 0;i < 5;i++)
	{
		m_MainTaskEasyStars[i] = GetButton(1010 + i);
		m_MainTaskHardStars[i] = GetButton(1000 + i);
		m_SwitchTaskEasyStars[i]= GetButton(1110 + i);
		m_SwitchTaskHardStars[i]= GetButton(1100 + i);	
	}	*/

	m_guiTab = GetTab(scTabID);
	if(m_guiTab)
	{
		AddControl2Tab(m_guiTab,0,scMainTaskListBoxID);
		AddControl2Tab(m_guiTab,0,scCancelMainTask);
		AddControl2Tab(m_guiTab,0,scMainTaskMlTextBoxID);

		AddControl2Tab(m_guiTab,1,scSwitchTaskListBoxID);
		AddControl2Tab(m_guiTab,1,scCancelSwtichTask);
		AddControl2Tab(m_guiTab,1,scSwitchTaskMlTextBoxID);

		AddControl2Tab(m_guiTab,2,scTaskKingMlTextBoxID);
		
		//添加任务难度按钮到页面
	/*	for(int i = 0;i< 5 ;i++)
		{
			m_guiTab->AddControl(0,m_MainTaskEasyStars[i]);
			m_guiTab->AddControl(0,m_MainTaskHardStars[i]);

			m_guiTab->AddControl(1,m_SwitchTaskEasyStars[i]);
			m_guiTab->AddControl(1,m_SwitchTaskHardStars[i]);
		}*/
		m_guiTab->SetVisible(false);
	}
	
	if (GetButton(  scTaskMainBtnID ))
	{ //soke 主线
		GetButton(  scTaskMainBtnID )->SetButtonGroup(1);
	//	GetButton(  scTaskMainBtnID )->SetVisible(false); //soke 删除主线按钮
	}
	if (GetButton( scTaskSwitchBtnID ))
	{ //soke 支线
		GetButton( scTaskSwitchBtnID )->SetButtonGroup(1);
	//	GetButton( scTaskSwitchBtnID )->SetVisible(false); //soke 删除支按钮
	}
	if (GetButton( scTaskKingBtnID ))
	{ //soke 检索任务
	//	GetButton( scTaskKingBtnID )->SetVisible(false); //soke 删除检索按钮
		GetButton( scTaskKingBtnID )->SetButtonGroup(1);
		GetButton( scTaskKingBtnID )->SetToolTips("检索当前可接任务的NPC");
	}

	ShowMainTaskStars(false,false,5);
	ShowMainTaskStars(true,false,5);
	ShowSwitchTaskStars(false,false,5);
	ShowSwitchTaskStars(true,false,5);
	//soke 删除支线和支线任务说明
//	m_pSwitchTaskList->SetVisible(false);
//	m_mlSwitchTaskTextBox->SetVisible(false);
	FUNCTION_END;
}

bool   CGuiTaskDialog::CancelTask()
{
	FUNCTION_BEGIN;	
	
	if(m_guiTab)
	{		
		CGuiListBox* pList = NULL;
		int index = m_guiTab->GetCurItem();
		switch(index)
		{
		case 0: pList = m_pMainTaskList; break;
		case 1: pList = m_pSwitchTaskList;break;
		default:break;
		}

		DWORD  dwTask = GetSelectTaskID(pList);
		if( dwTask == -1 ) return false;
		GetGameGuiManager()->AddMessageBox("确定要注销该任务吗？",eAction_CancelTask,(void*)dwTask,c_szMBOKCancel);
		return true;
	}
	return false;

	FUNCTION_END;
}
/**
 * \brief 更新任务状态
 * 
 * 先清空任务列表、修改为新的任务列表
 * 
 * \return void
 */
void CGuiTaskDialog::UpdateTaskContent()
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(!pMainChar) return ;
	CTask* ptask = pMainChar->GetTaskList();
	if(!ptask) return ;
	
	char	showInfo[128];
	//DWORD dwSelMainTaskid = GetSelectTaskID(m_pMainTaskList);
	//DWORD dwSelSwitchTaskid = GetSelectTaskID(m_pSwitchTaskList);

	//清空任务列表
	if(m_pMainTaskList) m_pMainTaskList->RemoveAllItems();
	if(m_pSwitchTaskList) m_pSwitchTaskList->RemoveAllItems();

	ShowMainTaskStars(false,false,5);
	ShowMainTaskStars(true,false,5);
	ShowSwitchTaskStars(false,false,5);
	ShowSwitchTaskStars(true,false,5);

	DWORD dwOldTaskShown = m_dwCurShownTask;
	ShowTaskText(0,m_mlMainTaskTextBox);
	ShowTaskText(0,m_mlSwitchTaskTextBox);

	//重新加载任务列表
	int		mainindex = 1;
	int     switchindex = 1;
	CTask::TaskList* plist = ptask->GetTask();
	//soke 任务界面修改
	sprintf(showInfo, " 主线任务");
	if(m_pMainTaskList) m_pMainTaskList->AddItem( showInfo, (void*)(-1) );
	if(m_pSwitchTaskList) m_pSwitchTaskList->AddItem( showInfo, (void*)(-1) );
	for( CTask::TaskListIt	it = plist->begin(); it != plist->end(); ++it )//主线
	{
		if(it->GetTaskName().length() > 0 || !(it->TaskShow) )
		{
			if(CTask::bIsMainTask(it->TaskID))//主线任务
			{
				sprintf(showInfo, "     %d.(%d级) %s ",mainindex++,it->TaskLevel,it->GetTaskName().c_str());
				if(m_pMainTaskList) m_pMainTaskList->AddItem( showInfo, (void*)(it->TaskID) );
				//soke 加这段就是 主线任务也显示支线的（HT是取消支线任务才整合在一起的）
			//	if(m_pSwitchTaskList) m_pSwitchTaskList->AddItem( showInfo, (void*)(it->TaskID) );
			}
		}
	}
	sprintf(showInfo, " 支线任务");
	if(m_pMainTaskList) m_pMainTaskList->AddItem( showInfo, (void*)(-1) );
	if(m_pSwitchTaskList) m_pSwitchTaskList->AddItem( showInfo, (void*)(-1) );
	for( CTask::TaskListIt	it = plist->begin(); it != plist->end(); ++it )//支线
	{
		if(it->GetTaskName().length() > 0 || !(it->TaskShow) )
		{
			if(CTask::bIsSwitchTask(it->TaskID))//分支任务
			{
				sprintf(showInfo, "     %d.(%d级) %s ",switchindex++,it->TaskLevel,it->GetTaskName().c_str());
				//soke 加这段就是 支线任务显示也显示主线的（HT是取消支线任务才整合在一起的）
			//	if(m_pMainTaskList) m_pMainTaskList->AddItem( showInfo, (void*)(it->TaskID) );
				if(m_pSwitchTaskList) m_pSwitchTaskList->AddItem( showInfo, (void*)(it->TaskID) );
			}
		}
	}
	///////////////////////////////////////////////////////////////////////
	if(dwOldTaskShown == 0) return ;
	if(CTask::bIsMainTask(dwOldTaskShown))
	//刷新此前显示的主线任务	
	{
		int i = bIsTaskIdShown(dwOldTaskShown,m_pMainTaskList);
		if( i != -1)
		{
			if(m_pMainTaskList) m_pMainTaskList->SetCurItem(i);
		}
		ShowTaskText(dwOldTaskShown,m_mlMainTaskTextBox);
	}
	else if(CTask::bIsSwitchTask(dwOldTaskShown))
	//刷新此前显示的分支任务	
	{
		int i = bIsTaskIdShown(dwOldTaskShown,m_pSwitchTaskList);
		if( i != -1)
		{
			if(m_pSwitchTaskList) m_pSwitchTaskList->SetCurItem(i);
		}
		ShowTaskText(dwOldTaskShown,m_mlSwitchTaskTextBox);
	}

	FUNCTION_END;
}


static stPointI sGotoNpcPos;
static void OnGotoNpcFinal(void* pParam)
{
	CNpc* pNpc = GetScene()->GetObjectAtGrid<CNpc>(sGotoNpcPos);
	if(pNpc)
		GetScene()->GetMainCharacter()->CallNpc(pNpc);
}

/**
 * \brief GUI消息处理函数
 * 
 * 分发listbox 选项改变消息
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiTaskDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_LISTBOX_SELECTION:
		if( OnListBoxSelChange(nID) ) return true;
		break;
	case EVENT_BUTTON_CLICKED:
		if( OnBtnClick(nID) ) return true;
		break;
	case EVENT_HYPERLINK_CLICKED:
		{
			const char* str = (const char*)(LPVOID)nID;
			char buf[MAX_PATH];
			int x = 0,y = 0;
			sscanf(str,"%s %d,%d",buf,&x,&y);			
			if( 0 == strcmp(buf,"goto") )
			{
				sGotoNpcPos = stPointI(x,y);
				GetScene()->GetMainCharacter()->Goto(stPointI(x,y),OnGotoNpcFinal);
			}
			return true;
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

//soke 增加任务文字显示
HRESULT CGuiTaskDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	m_pIcon = NULL;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	// Title
	int iOldFont = GetDevice()->SetFont(1);
	int x = 80;
	int y = 1;
	stRectI rect( 210 + x, 6 + y, 370 + x, 30 + y );

	rect.OffsetRect(m_x,m_y);
	GetDevice()->DrawString( "任务", rect );
	GetDevice()->SetFont( iOldFont );

	return hr;
}

bool   CGuiTaskDialog::OnBtnClick(int nID)
{
	FUNCTION_BEGIN;
	
	switch(nID)
	{
	case 100: //soke 新关闭按钮
	{
		SetVisible(false);
	}
	break;

	case scCancelMainTask:
	case scCancelSwtichTask: return CancelTask(); break;
	case scTaskMainBtnID:
		{
			GetButton(scTaskMainBtnID)->SetChecked(true);
			GetTab(scTabID)->SetCurItem(0);
		//	stResourceLocation st("data\\interfaces.gl",21,6);
			stResourceLocation st("data\\interfaces1.gl",0,1);
			GetImage(0)->SetImage(st);
			UpdateTaskContent();
			return true;
		}
		break;
	case scTaskSwitchBtnID:
		{
			GetButton(scTaskSwitchBtnID)->SetChecked(true);
			GetTab(scTabID)->SetCurItem(1);
		//	stResourceLocation st("data\\interfaces.gl",21,6);
		    stResourceLocation st("data\\interfaces1.gl",0,1);
			GetImage(0)->SetImage(st);
			UpdateTaskContent();
			return true;
		}
		break;
	case scTaskKingBtnID:
		{			
			/*for(int i = 0;i < 5;i++)
			{
				if( m_MainTaskHardStars[i]) 
				{
					m_MainTaskHardStars[i]->SetVisible(false);
				}
				if( m_MainTaskEasyStars[i]) 
				{
					m_MainTaskEasyStars[i]->SetVisible(false);
				}				
			}*/
			GetButton(scTaskKingBtnID)->SetChecked(true);
			GetTab(scTabID)->SetCurItem(2);
			//soke 检索任务
		//	stResourceLocation st("data\\interfaces.gl",21,20);
			stResourceLocation st("data\\interfaces1.gl",0,1);
			GetImage(0)->SetImage(st);
			//UpdateTaskContent();
			
			//stReqValidQuestUserCmd	cmd;
			//SEND_USER_CMD(cmd);
			//TRACE("____检索任务王命令___wyc");
			
			return true;
		}
		break;
	}
	return false;

	FUNCTION_END;
}

/**
 * \brief 发送请求变量数据消息
 * 
 * \param params : 变量描述
 * \return void
 */
void CGuiTaskDialog::RequestCmd(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	if( params.size() > 1 )
	{
		stRequestQuestUserCmd	cmd;

		_snprintf(cmd.target,16,params[0].c_str());
		cmd.target[15]=0;
		cmd.id = atoi( params[1].c_str() );
		cmd.offset = atoi( params[2].c_str() );

		SEND_USER_CMD(cmd);
	}

	FUNCTION_END;
}

/**
 * \brief 查找任务信息
 * 
 * 当列表框选项改变时，在编辑框显示对应的任务描述和任务等级
 * 
 * \return void
 */
bool CGuiTaskDialog::OnListBoxSelChange(int nID)
{
	switch(nID)
	{
	case scMainTaskListBoxID:
		{
			DWORD  id = GetSelectTaskID(m_pMainTaskList);
			if(id != -1) ShowTaskText(id,m_mlMainTaskTextBox);
			return true;
		}
	case scSwitchTaskListBoxID: 
		{
			DWORD  id = GetSelectTaskID(m_pSwitchTaskList);
			if(id != -1) ShowTaskText(id,m_mlSwitchTaskTextBox);
			return true;
		}
		break;
	}
	return false;
}


/**
 * \brief 设置任务等级
 * 
 * 根据任务的难度等级和角色的等级设置难易程度的显示
 * 
 * \param iLevel : 角色等级 - 难度等级
 * \return void
 */
void CGuiTaskDialog::SetLevel(int iLevel,bool bMainTask)
{
	FUNCTION_BEGIN;

	if (GetTab(scTabID)->GetCurItem() == 2)
	{
		return;
	}
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if( !pMainChar ) return ;
	int level = pMainChar->GetLevel() - iLevel;
	size_t size = (level > 0)?level:(-level);	
	
	int iNum = min(size,5);
	if(bMainTask)	//主线任务
	{
		ShowMainTaskStars(false,false,5);					//隐藏所有标志
		ShowMainTaskStars(true,false,5);					//隐藏所有标志

		if(level > 0)	ShowMainTaskStars(false,true,iNum,iLevel);	//显示iNum个简单标志
		else ShowMainTaskStars(true,true,iNum,iLevel);				//显示iNum个难标志
	}
	else			//分支任务
	{
		ShowSwitchTaskStars(false,false,5);					//隐藏所有标志
		ShowSwitchTaskStars(true,false,5);					//隐藏所有标志

		if(level > 0)	ShowSwitchTaskStars(false,true,iNum,iLevel);	//显示iNum个简单标志
		else ShowSwitchTaskStars(true,true,iNum,iLevel);				//显示iNum个难标志
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
void CGuiTaskDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

	//	m_pBtnClose->SetLocation(pt.x ,pt.y + 4);
		m_pBtnClose->SetLocation(pt.x - 5 ,pt.y - 88);
	}

	FUNCTION_END;
}


inline CTask* CGuiTaskDialog::GetTaskList()
{
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar) return NULL;
	return pMainChar->GetTaskList();
}

inline int CGuiTaskDialog::bIsTaskIdShown(DWORD dwTaskID,CGuiListBox* list)
{
	FUNCTION_BEGIN;
	if(!list) return -1;
	int num = list->GetCount();
	for(int i = 0 ;i < num ;++i)
	{
		DWORD pCurID = -1;
		void *	pData = list->GetItemData( i );
		if( pData!=NULL )
		{
			pCurID = (DWORD) pData;
			if( pCurID != 0 && pCurID == dwTaskID)
			{
				return i;
			}
		}		
	}
	return -1;
	FUNCTION_END;
}

void CGuiTaskDialog::ShowTaskText(DWORD dwTaskID,CGuiMLTextBox* ml)
{
	FUNCTION_BEGIN;

	if(ml)	ml->SetText( "" );
	m_dwCurShownTask = dwTaskID;
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if( !pMainChar ) return ;

	CTask* ptask = pMainChar->GetTaskList();
	if( !ptask ) return ;

	std::string	str = ptask->GetTaskShow(dwTaskID);	
	Debug_String(str.c_str());
	str = str_Replace(str,"(GetName)",pMainChar->GetName());
	if(ml)	ml->SetText( str.c_str() );

	int iLevel = ptask->GetTaskLevel(dwTaskID);
	if(iLevel != -1)
	{
		if(dwTaskID < 10000)
		{
			SetLevel(iLevel,true);//主线任务
		}
		else
		{
			SetLevel(iLevel,false);//分支任务
		}
	}
	FUNCTION_END;
}

DWORD  CGuiTaskDialog::GetSelectTaskID(CGuiListBox* list)
{
	FUNCTION_BEGIN;

	if(!list) return -1;
	int	index = list->GetCurItem();
	if( index==-1 ) return -1;

	DWORD	pCurID = -1;
	void *	pData = list->GetItemData( index );
	if( pData!=NULL )
	{
		pCurID = (DWORD) pData;
	}
	return pCurID;

	FUNCTION_END;
}


inline void   CGuiTaskDialog::ShowMainTaskStars(bool bHard,bool bShow,int num,int lv)
{
	FUNCTION_BEGIN;

	if(num > 5) num = 5;
	/*for(int i = 0;i < num;i++)
	{
		if( bHard && m_MainTaskHardStars[i]) 
		{
			m_MainTaskHardStars[i]->SetVisible(bShow);
		}
		if( !bHard && m_MainTaskEasyStars[i]) 
		{
			m_MainTaskEasyStars[i]->SetVisible(bShow);
		}
		if(bShow)
		{
			char level[MAX_PATH];
			sprintf(level,"任务难度:%d\n",lv);
			m_MainTaskEasyStars[i]->SetToolTips(level);
		}
	}
*/
	FUNCTION_END;
}

inline void   CGuiTaskDialog::ShowSwitchTaskStars(bool bHard,bool bShow,int num ,int lv)
{
	FUNCTION_BEGIN;

	//if(num > 5) num = 5;
	//for(int i = 0;i < num;i++)
	//{
	//	if( bHard && m_SwitchTaskHardStars[i]) 
	//	{
	//		m_SwitchTaskHardStars[i]->SetVisible(bShow);
	//	}
	//	if( !bHard && m_SwitchTaskEasyStars[i])
	//	{
	//		m_SwitchTaskEasyStars[i]->SetVisible(bShow);
	//	}
	//	if(bShow)
	//	{
	//		char level[MAX_PATH];
	//		sprintf(level,"任务难度:%d\n",lv);
	//		m_SwitchTaskEasyStars[i]->SetToolTips(level);
	//	}
	//}
}

void	CGuiTaskDialog::ShowLastTask(DWORD id)
{
	FUNCTION_BEGIN;

	if(CTask::bIsSwitchTask(id))
	{
		ShowTaskText(id,m_mlSwitchTaskTextBox);
		return ;
	}
	else if(CTask::bIsMainTask(id))
	{
		ShowTaskText(id,m_mlMainTaskTextBox);
		return ;
	}
}

void CGuiTaskDialog::UpdateTaskKing(const char* text)
{

	if(m_mlTaskKingTextBox)
	{
		m_mlTaskKingTextBox->SetText(text);
	}
}