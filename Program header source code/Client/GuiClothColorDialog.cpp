/**
*\file		GuiClothColorDialog.cpp
*\version	$ID$
*\author	$$$04公司
*			$$$04公司@###.com
*\date		2005-07-06 11:18:00
*\brief	服装换色对话框
*
* 跟服装师npc对话时显示的对话框，用于更换角色服装的颜色，移动slider时，角色的服装跟着变色。
*
* 确定则扣钱-换色，取消则退出，回到原来的颜色，
*
*/

#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\guiclothcolordialog.h"
#include "./GameScene.h"
#include "./RoleItem.h"
#include "GameScene.h"
#include "MainCharacter.h"

///r 分量slider id
static const int rSliderID = 4;

///g 分量slider id
static const int gSliderID = 5;

///b 分量slider id
static const int bSliderID = 6;

///颜色显示区域静态框id
static const int colorStaticID = 9;

///确定按钮id
static const int cIDOK = 20;

///取消按钮id
static const int cIDCancel = 21;


static const int cMoneyOfChangeClothColor = 1000;

static const stRectI cPriceRect(175,215,330,235);

/**
* \brief 构造函数
* 
*/
CGuiClothColorDialog::CGuiClothColorDialog(void)
{
	FUNCTION_BEGIN;

	m_pRSlider = NULL;
	m_pGSlider = NULL;
	m_pBSlider = NULL;
	m_pColorStatic = NULL;

	m_dwColor = 0xff000000;
	m_dwOldBodyColor = 0xff000000;

	FUNCTION_END;
}

/**
* \brief 析构函数
* 
*/
CGuiClothColorDialog::~CGuiClothColorDialog(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
* \brief "创建消息"响应函数
* 
* 初始化各个控件(slider，colorStatic)，设置初始颜色
* 
* \return void
*/
void CGuiClothColorDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	m_ptIcon.x = 36;
	m_ptIcon.y = 110;

	m_pRSlider = GetSlider(rSliderID);
	m_pGSlider = GetSlider(gSliderID);
	m_pBSlider = GetSlider(bSliderID);

	m_pColorStatic = GetStatic(colorStaticID);

	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar) 
	{
		m_dwColor = pMainChar->GetBodyCustomColor();
		SetDefaultColor(m_dwColor);
		m_dwOldBodyColor = m_dwColor;
	}
	CGuiDialog::OnCreate();

	FUNCTION_END;
}

HRESULT CGuiClothColorDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender( fElapsedTime );
	stRectI rect(cPriceRect);
	rect.OffsetRect(m_x,m_y);
	const char* Money = Money2String(cMoneyOfChangeClothColor);
	int iFont = GetDevice()->SetFont(1);
	GetDevice()->DrawString(Money,rect,0xff00ff00,DT_LEFT);	
	GetDevice()->SetFont(iFont);

	
	return hr;

	FUNCTION_END;
}

/**
* \brief GUI消息处理函数
* 
* 分发slider移动消息，处理按钮点击消息
* 
* \param nEvent : 消息句柄
* \param nID : 控件id
* \param pControl : 控件指针
* \return true if succeed else false
*/
bool CGuiClothColorDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_SLIDER_VALUE_CHANGED)	
		return OnSliderMove(nID);		//滑块移动事件

	if(nEvent == EVENT_BUTTON_CLICKED )
	{
		CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
		if(nID == cIDOK) 
		{			
			if(pMainChar)
			{
				size_t iMoney = pMainChar->GetMoney();//
				if(iMoney < cMoneyOfChangeClothColor)
				{
					//sorry your money is not enough
					GetGameGuiManager()->AddClientSystemMessage("对不起，你银子不够");
					return true;
				}
				else
				{
					stBodyColorPropertyUserCmd cmd;
					cmd.dwBodyColorCustom = m_dwColor;
					SEND_USER_CMD(cmd);					
					Close();
					return true;
				}
			}
		}
		else
		{
			if(pMainChar)
			{
				pMainChar->SetBodyCustomColor(m_dwOldBodyColor);
			}
			Close();
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
* \brief 根据color设置slider滑块的位置
* 
* \param color : 颜色
* \return void
*/
void CGuiClothColorDialog::SetColorOnSlider(DWORD color)
{
	FUNCTION_BEGIN;

	DWORD r = COLOR_GET_R(color);
	DWORD g = COLOR_GET_G(color);
	DWORD b = COLOR_GET_B(color);

	if(m_pRSlider && m_pGSlider && m_pBSlider)
	{
		m_pRSlider->SetValue(r);
		m_pGSlider->SetValue(g);
		m_pBSlider->SetValue(b);
	}

	FUNCTION_END;
}

/**
* \brief 设置颜色显示区域的颜色
* 
* \param color : 颜色
* \return void
*/
void CGuiClothColorDialog::SetColorOnColorWnd(DWORD color)
{
	FUNCTION_BEGIN;

	DWORD r = COLOR_GET_R(color);
	DWORD g = COLOR_GET_G(color);
	DWORD b = COLOR_GET_B(color);

	if(m_pColorStatic)	m_pColorStatic->SetBackGroundColor(color);

	FUNCTION_END;
}

/**
* \brief 设置对话框显示时默认的颜色
* 
* \param color : 颜色
* \return void
*/
void CGuiClothColorDialog::SetDefaultColor(DWORD color)
{
	FUNCTION_BEGIN;

	SetColorOnColorWnd(color);
	SetColorOnSlider(color);

	FUNCTION_END;
}

/**
* \brief "关闭"消息响应函数
* 
* 退出对话框、更新管理器对应的指针、关闭npc对话框
* 
* \return void
*/
void CGuiClothColorDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiClothColorDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();
	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		pMainChar->SetBodyCustomColor(m_dwOldBodyColor);
	}

	FUNCTION_END;
}

/**
* \brief "Slider Move"消息响应函数
* 
* 移动slider时获得当前的颜色值，保存并更新角色衣服的颜色
* 
* \param nID : slider 的id
* \return true if succeed else false
*/
bool CGuiClothColorDialog::OnSliderMove(UINT nID)
{
	FUNCTION_BEGIN;

	int r = COLOR_GET_R(m_dwColor);
	int g = COLOR_GET_G(m_dwColor);
	int b = COLOR_GET_B(m_dwColor);

	switch(nID)
	{
	case rSliderID:		
		r = m_pRSlider->GetValue();
		break;
	case gSliderID:
		g = m_pGSlider->GetValue();
		break;
	case bSliderID:
		b = m_pBSlider->GetValue();
		break;
	default:
		return false;
		break;
	}
	m_dwColor = COLOR_ARGB(255,r,g,b);
	SetColorOnColorWnd(m_dwColor);

	CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)   pMainChar->SetBodyCustomColor(m_dwColor);
	else return false;

	return true;

	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
void CGuiClothColorDialog::OnSetCloseLocation()
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
