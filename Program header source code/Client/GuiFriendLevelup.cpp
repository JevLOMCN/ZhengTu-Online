
#include "public.h"
#include "./GuiFriendLevelup.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guiitem.h"
#include "./table.h"
#include "./RoleItem.h"
CGuiFriendLevelup::CGuiFriendLevelup()
{
	
}

CGuiFriendLevelup::~CGuiFriendLevelup()
{
}

void CGuiFriendLevelup::OnCreate()
{
	FUNCTION_BEGIN;
	//CGuiDialog::OnCreate();
	m_pStatic = GetStatic(1);
	m_pStatic->SetText("");
	m_dwPlayerID = 0;
	FUNCTION_END;
}


void CGuiFriendLevelup::OnClose(void)
{
	FUNCTION_BEGIN;
	
	GetGameGuiManager()->m_guiFriendLevelup = NULL;

	FUNCTION_END;
}



bool CGuiFriendLevelup::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 100:
				{
					stFriendCongratulateInfoUserCmd pCmdInfo;
					pCmdInfo.PlayerId = m_dwPlayerID;
					_snprintf(pCmdInfo.name,MAX_NAMESIZE,"%s",GetScene()->GetMainCharacter()->GetName());
					_snprintf(pCmdInfo.info,512,"恭喜恭喜，我也会立刻赶上来的。");
					SEND_USER_CMD(pCmdInfo);
					Close();
				
				}break;
			case 101:
				{
					stFriendCongratulateInfoUserCmd pCmdInfo;
					pCmdInfo.PlayerId = m_dwPlayerID;
					_snprintf(pCmdInfo.name,MAX_NAMESIZE,"%s",GetScene()->GetMainCharacter()->GetName());
					_snprintf(pCmdInfo.info,512,"你还差得远呢，赶紧多努努力吧！");
					SEND_USER_CMD(pCmdInfo);
					Close();
				}break;
			case 102:
				{
					stFriendCongratulateInfoUserCmd pCmdInfo;
					pCmdInfo.PlayerId = m_dwPlayerID;
					_snprintf(pCmdInfo.name,MAX_NAMESIZE,"%s",GetScene()->GetMainCharacter()->GetName());
					_snprintf(pCmdInfo.info,512,"呦，不错嘛，继续加油吧。");
					SEND_USER_CMD(pCmdInfo);
					Close();
				}break;
			}
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiFriendLevelup::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation()); 

		m_pBtnClose->SetLocation(pt.x+8 ,pt.y-82);
	}

	FUNCTION_END;
}

HRESULT CGuiFriendLevelup::OnRender(float fElapsedTime)
{
	HRESULT  hr = CGuiDialog::OnRender(fElapsedTime);

	

	return hr;
}

void CGuiFriendLevelup::SetInfo(DWORD playerid,const char *name,DWORD playerlevel,DWORD type)
{
	char c[512];
	if (type==1)
	{
		_snprintf(c,512,"你的好友 %s 刚刚升到了%d级，赶快来恭喜他一下吧。",name,playerlevel);
	}
	if (type==2)
	{
		_snprintf(c,512,"你的好友 %s 刚刚升到了%d级，赶快来恭喜她一下吧。",name,playerlevel);
	}
	m_pStatic->SetText(c);
	m_dwPlayerID = playerid;
}