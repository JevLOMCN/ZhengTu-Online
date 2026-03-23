
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include ".\guiminimap.h"
#include ".\guimain.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiTaskHelpGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

//sky 任务追踪
stPointI sGotoNpcPos;
void OnGotoNpcFinal(void* pParam)
{
	CNpc* pNpc = GetScene()->GetObjectAtGrid<CNpc>(sGotoNpcPos);
	if(pNpc)
		GetScene()->GetMainCharacter()->CallNpc(pNpc);
}

CGuiTaskHelpGameDlg::CGuiTaskHelpGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	
	m_bCloseIsHide = true; //sky 任务跟踪

	FUNCTION_END;
}
 
CGuiTaskHelpGameDlg::~CGuiTaskHelpGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
* \brief 初始化特殊控件属性
* 
* 
* 
* \return 
*/
void CGuiTaskHelpGameDlg::InitCustomElements()
{
	FUNCTION_BEGIN;
//GetGameGuiManager()->m2_guiTaskHelpGame->GetImage( 0 )->SetLocation( GetDevice()->GetWidth() - 290, 110 );

	const char* szGrpFileName = "data\\interfaces.gl";
	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 0;
	rl.frame = 0;

	stRectI rcTexture;
	CGuiElement Element;


	//------------------------------------
	// Element for the 3D Border ScrollBar(滚动条)
	//------------------------------------
	{
		stRectI rects[9];
		rl.group = 11;
		rl.frame = 11;
		stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
		pInfo->rcTexture.SetRect(0,0,10,153);
		pInfo->ptLeftTop = stPointI(3,15);
		pInfo->ptRightBottom = stPointI(3,15);
		pInfo->ptBorderLeftTop = stPointI(-8,-4);
		pInfo->ptBorderRightBottom = stPointI(-8,-7);
		pInfo->CuttRect9(rects);
		m_nSBWidth = pInfo->rcTexture.Width();
		for(size_t i=0;i<count_of(rects);++i)
		{
			pInfo->element[i].SetTexture(&rl,&rects[i]);
			pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
			pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
		}
	}

	//-------------------------------------
	// CGuiScrollBar - Up Arrow
	//-------------------------------------
	rl.frame = -1;
	rcTexture.SetRect( 0, 0, 21, 21);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );


	rl.frame = -1;
	rcTexture.SetRect( 0, 0, 21, 21);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


	//-------------------------------------
	// CGuiScrollBar - Down Arrow
	//-------------------------------------
	rl.frame = -1;
	rcTexture.SetRect( 0,0 ,21, 21 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	rl.frame = -1;
	rcTexture.SetRect( 0,0 ,21, 21 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 3, &Element );

	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rl.frame = -1;
	rcTexture.SetRect( 0, 0, 14, 19 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 4, &Element );



	FUNCTION_END;
}

void CGuiTaskHelpGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	UpdateLocation(true);

	//OnResetScreenSize();
	

	//m_x = (  GetDevice()->GetWidth() - 204 - GetImage( 0 )->GetWidth() );
	//m_y = 250;

	
	//sky 任务追踪占用
	//m_width = 224;
	//m_height = 345;
 
	FUNCTION_END;
}

void CGuiTaskHelpGameDlg::UpdateLocation(bool bReAlign)
{
	FUNCTION_BEGIN;
	//soke 右上角的小地图整体位置
	//m_y = 126;
	//m_x = GetDevice()->GetWidth() - 204;

	m_x =   GetDevice()->GetWidth() - 290 ;
	m_y = 100;

	m_width = 304;
	m_height = 205;

	//soke 任务追踪可以寻路（在高就不能单击加属性、技能图标）
	m_width = 304;
	m_height = 365;


	FUNCTION_END;
}

bool CGuiTaskHelpGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_HYPERLINK_CLICKED: //sky 任务追踪
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
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 4:  //关
			{
				GetButton(5)->SetVisible(true);
				GetButton(4)->SetVisible(false);

				if (nID == 4) //信息滚动开关
				{
					m_bCloseIsHide = !m_bCloseIsHide;
					GetButton(4)->SetChecked(!GetButton(4)->GetChecked());
				}
			}
			break;
		case 5:  //开
			{
				GetButton(5)->SetVisible(false);
				GetButton(4)->SetVisible(true);

				if (nID == 5) //信息滚动开关
				{
					m_bCloseIsHide = !m_bCloseIsHide;
					GetButton(5)->SetChecked(!GetButton(5)->GetChecked());
				}
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param fElapsedTime : 描述
* \return 返回值的描述
*/
HRESULT CGuiTaskHelpGameDlg::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	//sky
	static DWORD timexyxWalk = 0;
	if (timexyxWalk == 0)
	{
		timexyxWalk = xtimeGetTime();
	}
	else if (xtimeGetTime() - timexyxWalk >=  10 *1000  ) //10秒
	{
		GetImage( 7 )->SetVisible( false );
	}
	
	if (m_bCloseIsHide)
	{
		//DrawScreenRightBottomMsg(point);
		//GetMLTextBox(6)->SetText(m_strQuestTarget.c_str());
		//GetStatic(3)->SetText(m_strQuestTarget.c_str());

		if (GetMLTextBox(6))
		{
			//GetMLTextBox(6)->SetFont(0);
			CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
			if (!pMainChar) return 0;
			CTask* ptask = pMainChar->GetTaskList();
			if (!ptask) return 0;

			CTask::TaskList* plist = ptask->GetTask();
			std::string	str = "";
			std::string sBegin = "";
			std::string s = "";
			int iBegin = 0;
			int i = 0;
			int n = 0;

			for (CTask::TaskListIt it = plist->begin(); it != plist->end(); ++it)
			{
				if (it->GetTaskName().length() > 0 || !(it->TaskShow))
				{
					str = ptask->GetTaskShow(it->TaskID);

					iBegin = str.find("<body>");
					iBegin += 6;
					sBegin = str.substr(0, iBegin);

					i = str.find("</p>", iBegin);
					i = str.find("</p>", i + 2);

					if ((int)str.find("</p>", i + 2) < 0)
					{
						continue;
					}

					if ((int)str.find(">/") > -1)
					{
						i = str.find("</p>", i + 2);
					}
					s += str.substr(iBegin, i + 4 - iBegin);
					++n;

					if (n > 4)
					{
						break;
					}
				}
			}

			str = sBegin + s + "</body>";

			if ((int)s.size() < 1)
			{
				str = "";
			}
			//DWORD dwColor = 0xff00ff00;
			str = str_Replace(str, "(GetName)", pMainChar->GetName());

			GetMLTextBox(6)->SetText(str.c_str());
			//GetMLTextBox(6)->m_VScrollBar.SetVisible(false);

			/*FILE* p = fopen("a.htm", "r");
			if (p)
			{
			str = "";

			while (!feof(p))
			{
			char c[1024] = "";

			fread(c, 1024, 1, p);
			str += c;
			}
			fclose(p);

			GetMLTextBox(6)->SetText(str.c_str());
			}
			else
			{
			//GetMLTextBox(6)->SetVisible(false);
			//GetMLTextBox(6)->SetVisible(m_bCloseIsHide);
			}*/
		}
	}

	GetMLTextBox(6)->SetVisible(m_bCloseIsHide);

	return S_OK;

	FUNCTION_END;
}



void CGuiTaskHelpGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTaskHelpGame = NULL;

	FUNCTION_END;
}







