#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"

#include ".\guiunoinpower.h"

struct stPower{
	char szText[MAX_NAMESIZE];
	int id;
};

stPower powerList[] = 
{
//	{"改变称号",CHANGE_ALIAS},
//	{"招收成员",ADD_MEMBER},
///	{"开除成员",FIRE_MEMBER},
	{"普通会员",GENERAL},
	{"发起或接受帮会战",DARE},
	{"帮主权限",MASTER},
	{"发公告",NOTIFY},
};

int GetPowerListSize()
{
	static int size = sizeof(powerList)/sizeof(stPower);

	return size;
}

CGuiUnoinPower::CGuiUnoinPower(char* name)
{
	memcpy(m_szName,name,sizeof(m_szName));
}


void CGuiUnoinPower::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiUnoinPower = NULL;
	/*GetGameGuiManager()->OnNpcActionEnd();
	GetGameGuiManager()->EndNpcDialog();*/

	FUNCTION_END;
}

void CGuiUnoinPower::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBox1 = GetListBox(2);
	m_pListBox2 = GetListBox(3);

	GetStatic(4)->SetText(m_szName);
	GetStatic(5)->SetText(m_szName);

	FUNCTION_END;
}

void   CGuiUnoinPower::RefreshData()
{
	if ((m_pListBox1==NULL)||(m_pListBox2==NULL))
		return;

	m_pListBox1->RemoveAllItems();
	m_pListBox2->RemoveAllItems();

	for (int i=0;i<GetPowerListSize();i++)
	{
		if (isset_state(m_byPower,powerList[i].id))
			m_pListBox2->AddItem(powerList[i].szText,&(powerList[i]));
		else
			m_pListBox1->AddItem(powerList[i].szText,&(powerList[i]));
	}
}

void CGuiUnoinPower::SetPower(byte* byPower)
{
	memcpy(m_byPower,byPower,sizeof(m_byPower));

	RefreshData();

}

bool CGuiUnoinPower::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	CGuiDialog* pDlg = NULL;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 10:
				{
					stUnionMemberPower  cmd;
					strncpy(cmd.name,m_szName,sizeof(cmd.name));
					cmd.byType = SET_UNION_MEMBER_POWER;
					memcpy(cmd.byPower,m_byPower,sizeof(cmd.byPower));

					SEND_USER_CMD(cmd);
				}
			case 11:
				{
					OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				}
				break;
			case 12:  //下按钮
				{
					if ((m_pListBox1)&&(m_pListBox1->GetCurItem()>=0))
					{
						stPower* p=(stPower*)m_pListBox1->GetItemData(m_pListBox1->GetCurItem());
						if (p)
						{
							if (p->id==MASTER)
								GameMessageBox("设置了[帮主权限]表示你将把帮主之位让给对方！");
							set_state(m_byPower,p->id);
							RefreshData();
						}
					}
				}
				break;
			case 13: //上按钮
				{
					if ((m_pListBox2)&&(m_pListBox2->GetCurItem()>=0))
					{
						stPower* p=(stPower*)m_pListBox2->GetItemData(m_pListBox2->GetCurItem());
						if (p)
						{
							clear_state(m_byPower,p->id);
							RefreshData();
						}
					}
				}
				break;
			}
		}
		break;

	case EVENT_LISTBOX_SELECTION:
		{
			
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiUnoinPower::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );

		m_pBtnClose->SetLocation(pt.x,pt.y + 24);
	}

	FUNCTION_END;
}