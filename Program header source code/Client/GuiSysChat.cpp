#include "public.h"
#include "./GameAppation.h"
#include "./GameGuiManager.h"
#include "GuiChatOutput.h"
//#include "GuiFloatAccessKey.h"
#include ".\guisyschat.h"
#include "./GuiTeam.h"

const int g_nMaxChatWidth = 200;
const int g_nMaxChatHigh = 100;
const stPointI g_ptScale(1,1);
const float g_fStopTime = 60.0f;
const float g_fMoveTime = 3.0f;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiSysChat::CGuiSysChat(void)
{
	FUNCTION_BEGIN;

	m_listSysChat.empty();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSysChat::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSysChat = NULL;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSysChat::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiSysChat::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	HRESULT  hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szChat : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSysChat::InsertSysChat(const char* szChat,DWORD color)
{
	FUNCTION_BEGIN;

	stSysChat st;

	strncpy(st.szChat,szChat,sizeof(st.szChat));
	st.size = GetDevice()->GetStringSize(szChat,-1,g_nMaxChatWidth);
	st.color = color;

	for (tListSysChat::iterator it = m_listSysChat.begin(); it != m_listSysChat.end(); ++it)
	{
		it->height += st.size.cy;
	}

	m_listSysChat.push_back(st);	

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSysChat::Draw(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	if (m_listSysChat.empty())
		return ;


	GetDevice()->SetFontScale(g_ptScale.x,g_ptScale.y);

	stRectI rcOut;

	int y = GetY();
	int x  = GetX();
	//if( GetGameGuiManager()->m_guiTeam )
	//{
	//	x = GetGameGuiManager()->m_guiTeam->GetRightX() + 10;
	//}
	//
	for (tListSysChat::iterator it = m_listSysChat.begin(); it != m_listSysChat.end(); ++it)
	{
		
		stSysChat& t = (*it);

		rcOut.bottom = y - t.height;
		rcOut.left = x;
		rcOut.right = rcOut.left + t.size.cx;
		rcOut.top = rcOut.bottom - t.size.cy;

		t.time += fElapsedTime;
		
		float  fTime = g_fMoveTime + g_fStopTime - t.time;
		fTime = fTime > 0.0f ? fTime = fTime : fTime = 0.01f;
		int alpha = (t.time<g_fStopTime)?255:fTime * 255 / g_fMoveTime;

		t.color &= 0x00ffffff;
		t.color |= alpha << 24;

		DWORD dwColor2 = COLOR_ARGB(alpha,0,0,0);
		if (rcOut.top>90)
			GetDevice()->DrawString(t.szChat,rcOut,t.color,DT_TOP | DT_LEFT,FontEffect_Border,dwColor2);
		
	}

	tListSysChat::iterator it1;
	for(it1 = m_listSysChat.begin() ; it1 != m_listSysChat.end() && (*it1).time >= (g_fStopTime + g_fMoveTime) ; ++it1)
	{}

	if( it1 != m_listSysChat.begin())
		m_listSysChat.erase(m_listSysChat.begin(),it1);

	
	GetDevice()->SetFontScale(1,1);


	FUNCTION_END;
}