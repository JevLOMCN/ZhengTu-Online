#include "public.h"
#include"GuiTastCueDialog.h"

CGuiTastCueDialog::CGuiTastCueDialog():m_Ttime(0),m_Ltime(0),IsHotKey(0),m_Alpha(255)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
};

void CGuiTastCueDialog::SetVisibleTime(float t)
{
	FUNCTION_BEGIN;
    m_Ltime = t;
	 this->SetVisible(true);
	//this->SetVisible(true);
	//m_Ltime = timeGetTime();
	//if(m_Ltime>=4000)
	//{
 //     
	//}
	FUNCTION_END;
}

void CGuiTastCueDialog::SetVisibleHotKey(int b)
{
	FUNCTION_BEGIN;
    IsHotKey = b;

	FUNCTION_END;
}

void CGuiTastCueDialog::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	//this->GetBackImage()->SetTextColor(COLOR_ARGB(60,74,233,29));

	this->SetLocation(320,450);
	GetStatic(2)->SetLocation(75,50);
	GetStatic(2)->SetTextColor(COLOR_ARGB(255,74,233,29));

	stResourceLocation rl2( "data\\Interfaces1.gl",1,80 );	
	m_pHotKeyBmp = GetDevice()->FindBitmaps( &rl2 );
	FUNCTION_END;
}

HRESULT CGuiTastCueDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	//HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);
	//m_pHotKeyBmp->Render(0,500,100,COLOR_ARGB(125,32,38,120),Blend_Null);//Ìí¼Ó¿ì½Ý¼üÍ¼Æ¬			

	if(m_Ltime>0)
	{
		m_Ttime += fElapsedTime;
		//this->SetVisible(true);
		if(IsHotKey>0&&m_pHotKeyBmp)//Ìí¼Ó¿ì½Ý¼üÍ¼Æ¬
		{
		  m_pHotKeyBmp->Render(0,500,100,-1,Blend_Null);//Ìí¼Ó¿ì½Ý¼üÍ¼Æ¬
		}//Ìí¼Ó¿ì½Ý¼üÍ¼Æ¬
		if(m_Ttime>=m_Ltime)
		{
			m_Alpha = m_Alpha-(fElapsedTime*70);
			if(m_Alpha<=0)
			{
				m_Alpha = 0;
			}
          	this->GetBackImage()->SetImageColor(0,COLOR_ARGB(m_Alpha,255,255,255));//wen
			GetStatic(2)->SetTextColor(COLOR_ARGB(m_Alpha,74,233,29));
			if(m_Ttime>=(m_Ltime+3))
			{
				m_Ltime=0;
				m_Ttime=0;
				this->SetVisible(false);
				m_Alpha = 255;

				IsHotKey=0;//Ìí¼Ó¿ì½Ý¼üÍ¼Æ¬
			}
		}
      
	}
	HRESULT hr =  CGuiDialog::OnRender(fElapsedTime);
	return hr;
	FUNCTION_END;
}