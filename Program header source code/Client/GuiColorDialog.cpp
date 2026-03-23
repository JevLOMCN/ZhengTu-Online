/**
 *\file		GuiColorDialog.cpp
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 09:35:00
 *\brief	颜色选择对话框
 *
 * 在GameGuiManager.h内的enumGuiActionType里定义一个响应类型(类似eAction_Color_Default)
 *
 * 用GetGameGuiManager()->AddColorDialog函数就可以添加一个颜色选择对话框
 *
 * 然后在bool CGuiColorDialog::OnAction函数里添加对应类型的响应代码即可。
 *
 */


#include "public.h"
#include "./GameGuiManager.h"
#include "./MainCharacter.h"
#include "./GameScene.h"
#include "GuiColorDialog.h"

///确定按钮id
static const int cColorDlgIDOK = 8;

///取消按钮id
static const int cColorDlgIDCancel = 9;


/**
 * \brief 构造函数
 * 
 * 根据响应类型和属主指针创建颜色选择对话框
 * 
 * \param Action : 响应类型 
 * \param pData : 属主指针
 * \return 
 */
CGuiColorDialog::CGuiColorDialog(enumGuiActionType Action/* = eAction_Color_Default*/,
								 void *pData /*= NULL*/)
{
	FUNCTION_BEGIN;

	m_eAction = Action;
	m_pData = pData;
	m_bSmDialog = true;
	
	FUNCTION_END;
}

/**
 * \brief 析构函数
 *
 * \return 
 */
CGuiColorDialog::~CGuiColorDialog(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief "创建消息"响应函数
 * 
 * 初始化各个控件(slider，lable、colorStatic)，设置初始颜色
 * 
 * \return void
 */
void CGuiColorDialog::OnCreate(void)
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pRSlider = GetSlider(2);
	m_pGSlider = GetSlider(3);
	m_pBSlider = GetSlider(4);


	m_pRLable = GetStatic(5);
	m_pGLable = GetStatic(6);
	m_pBLable = GetStatic(7);

	m_pColorStatic = GetStatic(1);
	m_pColorStatic->SetBackGroundColor(0xffffffff);

	FUNCTION_END;
}

/**
 * \brief GUI消息处理函数
 * 
 * 分发slider移动消息，分发按钮点击消息
 * 
 * \param nEvent : 消息句柄
 * \param nID : 控件id
 * \param pControl : 控件指针
 * \return true if succeed else false
 */
bool CGuiColorDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == 500)	return OnSliderMove(nID);		//滑块移动事件
	
	if(nEvent == 100 )	return OnButtonClick(nID);		//点击按钮事件

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
* \brief 设置对话框显示时默认的颜色
* 
* \param color : 颜色
* \return void
*/
void CGuiColorDialog::SetDefaultColor(DWORD color)
{
	FUNCTION_BEGIN;

	SetColorOnColorWnd(color);
	SetColorOnSlider(color);
	SetColorOnEdits(color);

	FUNCTION_END;
}

/**
 * \brief 获取当前选择的颜色
 * 
 * \return 当前颜色(DWORD)
 */
DWORD CGuiColorDialog::GetCurrentColor()
{
	FUNCTION_BEGIN;
	DWORD r = 0,g = 0,b = 0;
	sscanf(m_pRLable->GetText(),"%d",&r);
	sscanf(m_pGLable->GetText(),"%d",&g);
	sscanf(m_pBLable->GetText(),"%d",&b);
	return COLOR_ARGB(255,r,g,b);

	FUNCTION_END;
}

/**
* \brief 根据color设置slider滑块的位置
* 
* \param color : 颜色
* \return void
*/
void CGuiColorDialog::SetColorOnSlider(DWORD color)
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
void CGuiColorDialog::SetColorOnColorWnd(DWORD color)
{
	FUNCTION_BEGIN;
	
	DWORD r = COLOR_GET_R(color);
	DWORD g = COLOR_GET_G(color);
	DWORD b = COLOR_GET_B(color);

	if(m_pColorStatic)	m_pColorStatic->SetBackGroundColor(color);

//	TRACE("Draw rect:left%d,top:%d,right:%d,bottom%d,color:r%d,g%d,b%d,return:%d\n",
//		rci.left,rci.top,rci.right,rci.bottom,r,g,b,hr);

	FUNCTION_END;
}

/**
* \brief 根据color设置RGB颜色分量文字显示框显示的数字
* 
* \param color : 颜色
* \return void
*/
void CGuiColorDialog::SetColorOnEdits(DWORD color)
{
	FUNCTION_BEGIN;

	DWORD r = COLOR_GET_R(color);
	DWORD g = COLOR_GET_G(color);
	DWORD b = COLOR_GET_B(color);
	Assert(r >= 0 && r <= 255);
	Assert(g >= 0 && g <= 255);
	Assert(b >= 0 && b <= 255);	


	if(m_pRLable && m_pGLable && m_pBLable)
	{
		char szText[MAX_NAMESIZE];
		sprintf(szText,"%d",r);
		m_pRLable->SetText(szText);
		sprintf(szText,"%d",g);
		m_pGLable->SetText(szText);
		sprintf(szText,"%d",b);
		m_pBLable->SetText(szText);
	}	

	FUNCTION_END;
}

/**
* \brief "Slider Move"消息响应函数
* 
* 移动slider时获得当前的颜色值，在颜色显示区域显示
* 
* \param nID : slider 的id
* \return true if succeed else false
*/
bool CGuiColorDialog::OnSliderMove(UINT nID)
{
	FUNCTION_BEGIN;

	//TRACE("CGuiColorDialog::OnSliderMove%d",nID);	
	DWORD dwColor = GetCurrentColor();
	DWORD r = COLOR_GET_R(dwColor);
	DWORD g = COLOR_GET_R(dwColor);
	DWORD b = COLOR_GET_R(dwColor);
	char szText[MAX_NAMESIZE];
	switch(nID)
	{
	case 2:		
		r = m_pRSlider->GetValue();
		sprintf(szText,"%d",r);
		m_pRLable->SetText(szText);
		break;
	case 3:
		g = m_pGSlider->GetValue();
		sprintf(szText,"%d",g);
		m_pGLable->SetText(szText);
		break;
	case 4:
		b = m_pBSlider->GetValue();
		sprintf(szText,"%d",b);
		m_pBLable->SetText(szText);
		break;
	default:
		return false;
		break;
	}
	dwColor = GetCurrentColor();
	SetColorOnColorWnd(dwColor);
	return true;

	FUNCTION_END;
}

/**
 * \brief 选好颜色后根据这个颜色对话框的action类型作出相应的处理
 * 
 * 处理系统设置对话框的颜色选择动作，还可以增加别的类型处理
 *
 * \return 返回值的描述
 */
void CGuiColorDialog::OnAction()
{
	DWORD dwColor = GetCurrentColor();

	switch(m_eAction)
	{
	case eAction_Color_SystemSetting:	
		{
			CGuiStatic *pStatic = (m_pData)?((CGuiStatic *)m_pData):NULL;
			if(pStatic) 
			{
				pStatic->SetBackGroundColor(dwColor);
			}
		}
		break;
	case eAction_Color_Default:
		break;
	default:
		break;
	}
}

/**
 * \brief 按钮点击处理
 * 
 * \param nID : 按钮的id
 * \return false if id not exist
 */
bool CGuiColorDialog::OnButtonClick(UINT nID) 
{
	FUNCTION_BEGIN;

	//TRACE("----OnButtonClick:%d\n",nID);
	switch(nID)
	{
	case cColorDlgIDOK :	//确定		
		OnAction();
		break;
	case cColorDlgIDCancel:		//取消			
		break;
	case -1:	//关闭	
		break;
	default:
		return false;
		break;
	}	
	m_pData = NULL;
	SetVisible(false);
	return true;

	FUNCTION_END;
}