/**
 * \file      IDevice.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图形设备接口
 * 
 *	      图形设备接口
 */

#include "../include/engine.h"
#include "../include/IBitmap.h"
#include "../include/IDevice.h"
#include "../include/idevice.h"

const int c_nPicCharBase = 0xff80; 
const DWORD c_dwClrDefaultShadow = 0xe0000000;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param width : 描述
 * \param height : 描述
 * \param dwPixelBits : 描述
 * \param dwResult : 描述
 * \return 返回值的描述
 */
bool IDevice::GetNextLevelPixelBits(int width,int height,DWORD dwPixelBits,DWORD & dwResult)
{
	for(int i=m_aDisplayMode.size()-1;i>=0; --i)
	{
		if(m_aDisplayMode[i].dmPelsWidth == width && m_aDisplayMode[i].dmPelsHeight == height && m_aDisplayMode[i].dmBitsPerPel < dwPixelBits)
		{
			dwResult = m_aDisplayMode[i].dmBitsPerPel;
			return true;
		}
	}
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param width : 描述
 * \param height : 描述
 * \param szResult : 描述
 * \return 返回值的描述
 */
bool IDevice::GetNextLevelWindowSize(int width,int height,SIZE& szResult)
{
	for(int i=m_aDisplayMode.size()-1;i>=0; --i)
	{
		if( m_aDisplayMode[i].dmPelsWidth < width || (m_aDisplayMode[i].dmPelsWidth == width && m_aDisplayMode[i].dmPelsHeight < height) )
		{
			szResult.cx = m_aDisplayMode[i].dmPelsWidth;
			szResult.cy = m_aDisplayMode[i].dmPelsHeight;
			return true;
		}
	}
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pRet : 描述
 * \param nWidth : 描述
 * \param nHeight : 描述
 * \param dwPixelBits : 描述
 * \return 返回值的描述
 */ 
bool IDevice::GetDisplayMode(DEVMODE *pRet,DWORD nWidth,DWORD nHeight,DWORD dwPixelBits)
{

	if(nWidth < 800 || nHeight < 600)
	{
		nWidth = 800;
		nHeight = 600;
	}

	Debug_Int(nWidth);
	Debug_Int(nHeight);

	if(dwPixelBits < 16) dwPixelBits = 16;
	for(;;)
	{
		DWORD dwBitsPixel = dwPixelBits;
		for(;;)
		{
			for(size_t i=0;i<m_aDisplayMode.size();++i)
			{
				DEVMODE & dm = m_aDisplayMode[i];
				if(dm.dmBitsPerPel == dwBitsPixel && dm.dmPelsWidth == nWidth && dm.dmPelsHeight == nHeight)
				{
					*pRet = dm;
					return true;
				}
			}
			if(!GetNextLevelPixelBits(nWidth,nHeight,dwBitsPixel,dwBitsPixel))
				break;
		}
		SIZE szWindow;
		if(!GetNextLevelWindowSize(nWidth,nHeight,szWindow))
			break;
		nWidth = szWindow.cx;
		nHeight = szWindow.cy;
	}

	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
static int __cdecl compareDisplayMode (const void * p1, const void * p2) 
{
	DEVMODE* m1 = (DEVMODE*)p1;
	DEVMODE* m2 = (DEVMODE*)p2;
	if(m1->dmPelsWidth > m2->dmPelsWidth)
		return -1;
	if(m1->dmPelsWidth < m2->dmPelsWidth)
		return 1;
	if(m1->dmPelsHeight > m2->dmPelsHeight)
		return -1;
	if(m1->dmPelsHeight < m2->dmPelsHeight)
		return 1;

	if(m1->dmBitsPerPel > m2->dmBitsPerPel)
		return -1;
	if(m1->dmBitsPerPel < m2->dmBitsPerPel)
		return 1;

	if(m1->dmDisplayFrequency > m2->dmDisplayFrequency)
		return -1;
	if(m1->dmDisplayFrequency < m2->dmDisplayFrequency)
		return 1;

	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void IDevice::CheckDisplayMode()
{
	DWORD num = 0;
	DEVMODE mode;
	m_aDisplayMode.resize(0);
	while(1)
	{
		memset(&mode,0,sizeof(mode));
		mode.dmSize = sizeof(mode);
		if(!EnumDisplaySettings(NULL,num++,&mode))
			break;

		//if(	mode.dmPelsWidth >= 1024 && mode.dmBitsPerPel >= 16)

		// bug 810 仅保留 16 位色, 
		// 底层不要改动,系统设置去掉32位色,但手工改 config.ini 还是让他用32位色
		if( mode.dmPelsWidth >= 800 && mode.dmPelsHeight >= 600 )
		{
			bool bFind = false;
			for(size_t i=0;i<m_aDisplayMode.size();++i)
			{
				if(m_aDisplayMode[i].dmBitsPerPel == mode.dmBitsPerPel 
					&& m_aDisplayMode[i].dmPelsWidth == mode.dmPelsWidth && m_aDisplayMode[i].dmPelsHeight == mode.dmPelsHeight)
				{
					bFind = true;
				}
			}
			if(!bFind)
			{
				m_aDisplayMode.push_back(mode);
			}
		}
	}
	qsort(&m_aDisplayMode[0],m_aDisplayMode.size(),sizeof(DEVMODE),compareDisplayMode);
	for(size_t i=0;i<m_aDisplayMode.size();i++)
	{
		DEVMODE & dm = m_aDisplayMode[i];
		TRACE("DisplayMode [ Width:%u Height:%u BitsPixel:%u DisplayFrequency:%u ]\n",dm.dmPelsWidth,dm.dmPelsHeight,dm.dmBitsPerPel,dm.dmDisplayFrequency);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pBuf : 描述
 * \param count : 描述
 * \return 返回值的描述
 */
int IDevice::GetWindowSizeList(SIZE* pBuf,size_t count)
{
	int ret = 0;
	for(size_t i=0;i<m_aDisplayMode.size();i++)
	{
		int j;
		for(j=0;j<ret;++j )
		{
			if(pBuf[j].cx == m_aDisplayMode[i].dmPelsWidth && pBuf[j].cy == m_aDisplayMode[i].dmPelsHeight)
				break;
		}
		if(j == ret)
		{
			pBuf[ret].cx = m_aDisplayMode[i].dmPelsWidth;
			pBuf[ret].cy = m_aDisplayMode[i].dmPelsHeight;
			ret++;
		}
	}
	return ret;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pdwBits : 描述
 * \param count : 描述
 * \return 返回值的描述
 */
int IDevice::GetPixelBitsList(DWORD *pdwBits,size_t count)
{
	int ret = 0;
	for(size_t i=0;i<m_aDisplayMode.size();i++)
	{
		int j;
		for(j=0;j<ret;++j )
		{
			if(pdwBits[j] == m_aDisplayMode[i].dmBitsPerPel)
				break;
		}
		if(j == ret)
		{
			pdwBits[ret] = m_aDisplayMode[i].dmBitsPerPel;
			ret++;
		}
	}
	return ret;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pMode : 描述
 * \return 返回值的描述
 */
bool IDevice::BeginFullScreen(DEVMODE* pMode)
{
	DEVMODE mode = * pMode;
	mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT ;

	/*DEVMODE modReg;
	memset(&modReg,0,sizeof(modReg));
	modReg.dmSize = sizeof(modReg);
	modReg.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&modReg);

	if(DISP_CHANGE_SUCCESSFUL != ::ChangeDisplaySettings(&mode,CDS_TEST))
		mode.dmFields &= (~DM_DISPLAYFREQUENCY);*/

	TRACE("BeginFullScreen............\n");
	return (m_bFullScreen = (DISP_CHANGE_SUCCESSFUL == ::ChangeDisplaySettings(&mode,CDS_FULLSCREEN)));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool IDevice::EndFullScreen()
{
	if(!m_bFullScreen) return true;
	TRACE("EndFullScreen............\n");
	m_bFullScreen = false;
	return (DISP_CHANGE_SUCCESSFUL == ::ChangeDisplaySettings(NULL,0));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool IDevice::ResetWindowPos()
{
	if(GetWindowLong(m_hWnd,GWL_STYLE) & WS_CHILD)
		return true;

	HWND hWndInsertAfter;
	int x,y,cx,cy;
	DWORD dwExStyle;
	DWORD dwStyle;

	dwExStyle = GetWindowLong(m_hWnd,GWL_EXSTYLE);
	dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);

	if(m_dwFlags & DEVICE_FULLSCREEN){

		dwStyle &= (~WS_SIZEBOX);
		dwStyle &= (~WS_MAXIMIZEBOX);

		hWndInsertAfter = HWND_TOPMOST;

		stRectI rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = m_iWidth;
		rc.bottom = m_iHeight;

		AdjustWindowRectEx(&rc,dwStyle,GetMenu(m_hWnd) != NULL,dwExStyle);

		x = rc.left;
		y = rc.top;
		cx = rc.Width();
		cy = rc.Height();
		
	}else{

		dwStyle |= (WS_SIZEBOX);
		dwStyle |= (WS_MAXIMIZEBOX);
		dwStyle |= (WS_BORDER);
		dwStyle |= (WS_CAPTION);

		hWndInsertAfter = HWND_NOTOPMOST;

		int fullScreenWidth,screenHeight,fullScreenHeight;
		fullScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
		fullScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		stRectI rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = m_iWidth;
		rc.bottom = m_iHeight;

		AdjustWindowRectEx(&rc,dwStyle,GetMenu(m_hWnd) != NULL,dwExStyle);
		
		cx = rc.Width();
		cy = rc.Height();

		stRectI rWorkArea;
		BOOL bResult = SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rWorkArea, 0);
		if(!bResult)
		{
			//如果调用不成功就利用GetSystemMetrics获取屏幕面积
			rWorkArea.left = 0;
			rWorkArea.top = 0;
			rWorkArea.right = fullScreenWidth;
			rWorkArea.bottom = fullScreenHeight;
		}

		if(rWorkArea.Height() >= cy || rWorkArea.Height() == screenHeight){
			if(rWorkArea.Width() < rc.Width()){
				rc.left = 0;
				rc.right = m_iWidth;
				AdjustWindowRectEx(&rc,dwStyle,GetMenu(m_hWnd) != NULL,dwExStyle);
				x = rc.left;
			}else
				x = 0;
			y = 0;
		}else{
			rc.left = 0;
			rc.right = m_iWidth;
			rc.top = 0;
			rc.bottom = m_iHeight;
			AdjustWindowRectEx(&rc,dwStyle,GetMenu(m_hWnd) != NULL,dwExStyle);
			x = rc.left;
			y = rc.top;
		}
	}

	SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);

#ifdef _DEBUG
	BOOL b = SetWindowPos(m_hWnd,hWndInsertAfter,x,y,cx,cy,SWP_NOZORDER);
#else
	BOOL b = SetWindowPos(m_hWnd,hWndInsertAfter,x,y,cx,cy,0);
#endif
	UpdateWndSize();
	return b != 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param Message : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
bool IDevice::MsgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch(Message)
	{
	case WM_ACTIVATEAPP:
	//case WM_ACTIVATE:
		m_bActive = (wParam != WA_INACTIVE);
		//Engine_WriteLogF("WM_ACTIVATE %d ................\n",m_bActive);
		if(IsCreated())
		{
			if(m_dwFlags & DEVICE_FULLSCREEN)
			{
				if(m_bActive)
				{
					BeginFullScreen(&m_dm);
					ResetWindowPos();
				}
				else
				{
					EndFullScreen();
					SendMessage(m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);
				}
			}
		}
		break;
	case WM_MOVE:
		{
			POINT pos = {0,0};
			ClientToScreen(m_hWnd,&pos);
			m_iWndX = pos.x;
			m_iWndY = pos.y;
		}
		break;
	case WM_SIZE:
		{
			m_iWndWidth = (short)(lParam & 0xffff);
			m_iWndHeight = (short)(lParam >> 16);
		}
		break;
	}
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
IDevice::IDevice()
: m_hWnd(NULL)
, m_dwFlags(0)
, m_iWidth(0)
, m_iHeight(0)
, m_iWndWidth(0)
, m_iWndHeight(0)
, m_FlushType(Flush_Cover)
, m_eProgramType(eProgram_Game)
, m_bActive(false)
, m_bCreated(false)
, m_eImageScaleMode(ImageScaleMode_Half)
, m_bFullScreen(false)
, m_bRenderScene(false)
, m_iWndX(0)
, m_iWndY(0)
, m_bMonochrome(false)
{
	memset(&m_dm,0,sizeof(m_dm));
	memset(&m_rcRender,0,sizeof(m_rcRender));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prc : 描述
 * \return 返回值的描述
 */
RECT IDevice::SetClipRect(const RECT* prc)
{
	RECT rc = m_rcRender;
	m_rcRender = stRectI(0,0,m_iWidth,m_iHeight);
	if(prc)
	{
		m_rcRender &= *prc;
	}
	return rc;
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void IDevice::UpdateWndSize()
{
	stRectI rect;
	GetClientRect(m_hWnd,&rect);
	m_iWndWidth = rect.Width();
	m_iWndHeight = rect.Height();
}

void IDevice::ConverMousePoint(POINT & pt)
{
	if(GetFlags() & DEVICE_FULLSCREEN)
	{}
	else{
		if(GetWndWidth())
			pt.x = pt.x * GetWidth()/GetWndWidth();
		if(GetWndHeight())
			pt.y = pt.y * GetHeight()/GetWndHeight();
	}
}