#include "../include/engine.h"
#include "../include/Appation.h"
#include "../include/fps.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
stConfigBase::stConfigBase()
{ 
	screenWidth = 800;
	screenHeight = 600;
	colorBits = 16;
	bWindow = false;
	bVertSync = true;
	bLightMap = false;
	bWeather = false;
	bMultiTex = true;
	bRenderTarget = true;
	bUseDXT = true;

	bMainSound = true;			//主音效开关
	bSceneSound = true;			//场景音效开关
	bCircumstanceSound = true;	//环境音效开关
	bBackgroundSound = true;	//背景音效开关
	bHardwareCursor = true;

	nMainSound = 50;			//主音效音量
	nSceneSound = 50;			//场景音效音量
	nCircumstanceSound = 50;	//环境音效音量
	nBackgroundSound = 50;		//背景音效音量
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
DWORD stConfigBase::GetFlags()
{
	DWORD dwFlags = 0;
	if(!bWindow) dwFlags |= IDevice::DEVICE_FULLSCREEN;
	if(bVertSync) dwFlags |= IDevice::DEVICE_VSYN;
	if(bLightMap) dwFlags |= IDevice::DEVICE_LIGHTMAP;
	if(bMultiTex) dwFlags |= IDevice::DEVICE_MULTITEX;
	if(bRenderTarget) dwFlags |= IDevice::DEVICE_RENDERTARGET;
	if(bUseDXT) dwFlags |= IDevice::DEVICE_DXT;
	//if(colorBits == 32) dwFlags |= IDevice::DEVICE_32BPP;
	return dwFlags;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwFlags : 描述
 * \return 返回值的描述
 */
void stConfigBase::SetFlags(DWORD dwFlags)
{
	bWindow = ( 0 == (dwFlags & IDevice::DEVICE_FULLSCREEN));
	bVertSync = (0 != (dwFlags & IDevice::DEVICE_VSYN));
	bLightMap = (0 != (dwFlags & IDevice::DEVICE_LIGHTMAP));
	bUseDXT = (0 != (dwFlags & IDevice::DEVICE_DXT));
	//colorBits = (dwFlags & IDevice::DEVICE_32BPP) ? 32 : 16;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pDevice : 描述
 * \return 返回值的描述
 */
void stConfigBase::SetProperty(IDevice* pDevice)
{
	SetFlags(pDevice->GetFlags());
	screenWidth = pDevice->GetWidth();
	screenHeight = pDevice->GetHeight();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CAppation * g_pCurrentApp = NULL;
CAppation::CAppation(void)
{
	strcpy(m_szWndClassName,"Engine Demo");
	strcpy(m_szWndName,"Engine Demo");
	m_hIcon = NULL;
	m_hWnd = NULL;
	m_hInstance = (HINSTANCE)GetModuleHandle(NULL);
	m_eProgramType = eProgram_Game;
	g_pCurrentApp = this;
	m_szAppPath[0] = 0;
	m_dwThreadID = NULL;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CAppation::~CAppation(void)
{
	g_pCurrentApp = NULL;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param uMsg : 描述
 * \param wParam : 描述
 * \param lParam : 描述
 * \return 返回值的描述
 */
LRESULT CAppation::MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSKEYDOWN:
		if(GetKeyState(VK_MENU) < 0 || GetKeyState(VK_F10) < 0) 
			return 0;
		break;
	case WM_ERASEBKGND:
		return TRUE;
	}
	return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hwnd : 描述
 * \param uMsg : 描述
 * \param wParam : 描述
 * \param lParam							 : 描述
 * \return 返回值的描述
 */
static LRESULT CALLBACK WindowProc( 
							HWND hwnd,
							UINT uMsg,
							WPARAM wParam,
							LPARAM lParam
							)
{
	CAppation* pAppation  = (CAppation*)GetWindowLong(hwnd,GWL_USERDATA);
	if(pAppation)
		return pAppation->MsgProc(uMsg,wParam,lParam);

	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CAppation::InitInstance()
{
	//检测用户当前的桌面屏幕的分辨率信息
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	setlocale(LC_ALL,".ACP");

	m_dwThreadID = GetCurrentThreadId();

	if(FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED)))
		return false;

	GetModuleFileName(NULL,m_szAppPath,sizeof(m_szAppPath));
	*strrchr(m_szAppPath,'\\') = 0;

	Engine_Init();

	WSADATA tWsaData;
	memset(&tWsaData,0,sizeof(WSADATA));
	WORD wVersionRequested = MAKEWORD( 2, 0 );

	if(WSAStartup(wVersionRequested,&tWsaData))
	{
		//MessageBox( NULL,"SOCKET 初始化失败！",NULL,MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if (LOBYTE(tWsaData.wHighVersion) < 2)
	{ 
		//MessageBox( NULL,"SOCKET 版本错误！",NULL,MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	WNDCLASSEX wc;
	memset(&wc,0,sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = m_hIcon;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = m_szWndClassName;
	wc.hbrBackground = NULL;
	RegisterClassEx( &wc );

	DWORD dwStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX /*| WS_VISIBLE | WS_MAXIMIZEBOX | WS_SIZEBOX*/ ;
	//DWORD dwStyle = (WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX));
	DWORD dwExStyle = 0;
	stConfigBase* pConfig = GetConfigBase();
	stRectI rect(0,0,pConfig->screenWidth,pConfig->screenHeight);

	m_hWnd = CreateWindowEx(dwExStyle,m_szWndClassName,m_szWndName,dwStyle,rect.left,rect.top,rect.Width(),rect.Height(),NULL,NULL,m_hInstance,NULL);
	if(m_hWnd == NULL) return false;

	SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);

	DWORD dwFlags = pConfig->GetFlags();

	if(!GetDevice()->Create(m_hWnd,pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags,m_eProgramType))
		return false;

	pConfig->SetProperty(GetDevice());

#ifndef _DEBUG
	if (!(GetDevice()->GetFlags() & IDevice::DEVICE_FULLSCREEN))
	{
		pConfig->screenWidth = w;
		pConfig->screenHeight = h;
		stRectI rWorkArea;
		BOOL bResult = SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rWorkArea, 0);
		pConfig->screenWidth  = rWorkArea.right - rWorkArea.left;
		pConfig->screenHeight = rWorkArea.bottom - rWorkArea.top;
		GetDevice()->Reset(pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags);
	}
	else
	{
		if (pConfig->screenWidth >= w || pConfig->screenHeight >= h)
		{
		}
		else
		{
			pConfig->screenWidth = w;
			pConfig->screenHeight = h;
		}		
		GetDevice()->Reset(pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags);
	}	
#else 
	GetDevice()->Reset(pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags);
#endif 

	//ShowWindow(m_hWnd,SW_SHOW);
	//UpdateWindow(m_hWnd);
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
void CAppation::RefreshFrame(float fElapsedTime)
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
void CAppation::Draw(float fElapsedTime)
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool CAppation::AforeDraw()
{
	/*if((!GetDevice()->IsActive() && GetDevice()->GetFlags() & IDevice::DEVICE_FULLSCREEN) || IsIconic(m_hWnd))
		return false;*/
	IDevice* pDev = GetDevice();
	if( IsIconic(pDev->GetHWnd()) )
	{
		Sleep(1);
		return false;
	}

	if(pDev->GetFlags() & IDevice::DEVICE_FULLSCREEN){
		if(!pDev->IsActive()){
			Sleep(1);
			return false;
		}
	}else{
		HDC hDC = GetDC(m_hWnd);
		stRectI clipRect;
		GetClipBox(hDC,&clipRect);
		ReleaseDC(m_hWnd,hDC);
		if(clipRect.IsRectEmpty()){
			Sleep(1);
			return false;
		}
	}

	if(!GetDevice()->TestCooperativeLevel())
		return false;
	if(!GetDevice()->BeginScene())
		return false;
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CAppation::BeforeDraw()
{
	GetDevice()->EndScene();
	GetDevice()->Present();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
int CAppation::Run()
{
	MSG msg;
	for(;;)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				break;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else
		{

			Engine_GetFps()->BeginFrame();

			Engine_GetState()->Refresh();
			RefreshFrame(Engine_GetElapsedTime());

			if(AforeDraw())
			{
				Engine_GetState()->RefreshDraw();
				Draw(Engine_GetDrawElapsedTime());
				BeforeDraw();
			}

			Engine_GetFps()->EndFrame();
		}
	}
	return ExitInstance();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
int CAppation::ExitInstance()
{
	GetDevice()->Destroy();
	Engine_Uninit();
	WSACleanup();
	CoUninitialize();
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CAppation::ApplyConfig()
{
	stConfigBase* pConfig = GetConfigBase();
	if(!pConfig) return;

	DWORD dwFlags = pConfig->GetFlags();

	if ( !(GetDevice()->GetFlags() & IDevice::DEVICE_FULLSCREEN) )
	{
		//如果系统设置没有改变则不刷新
		if(GetDevice()->GetWidth() != pConfig->screenWidth ||
			GetDevice()->GetHeight() != pConfig->screenHeight ||
			GetDevice()->GetPixelBits() != pConfig->colorBits ||
			GetDevice()->GetFlags() != dwFlags)
		{		
			//检测用户当前的桌面屏幕的分辨率信息
			int w = GetSystemMetrics(SM_CXSCREEN);
			int h = GetSystemMetrics(SM_CYSCREEN);

			if (( w == pConfig->screenWidth ) && ( h == pConfig->screenHeight ))
			{
				//获得任务栏位置 上 下 左 右
				//获得任务栏taskbarW tasebarH
				stRectI rWorkArea;
				BOOL bResult = SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rWorkArea, 0);
				//taskbar in right
				if (( rWorkArea.left == 0 ) && ( rWorkArea.right - rWorkArea.left ) < w )
				{

				}
				//taskbar in left
				if (( rWorkArea.left != 0 ) && ( rWorkArea.right - rWorkArea.left ) < w )
				{

				}
				//taskbar in up
				if (( rWorkArea.top == 0 ) && ( rWorkArea.bottom - rWorkArea.top ) < h )
				{

				}
				//taskbar in down
				if (( rWorkArea.top != 0 ) && ( rWorkArea.bottom - rWorkArea.top ) < h )
				{

				}
				pConfig->screenWidth  = rWorkArea.right - rWorkArea.left;
				pConfig->screenHeight = rWorkArea.bottom - rWorkArea.top;
				/*
				APPBARDATA appbard;
				appbard.cbSize = sizeof(appbard);
				//appbard.lParam = ABS_AUTOHIDE;
				appbard.rc.left = 0,
				appbard.rc.top = 10;
				appbard.rc.right = 1000;
				appbard.rc.bottom = 1000;
				SHAppBarMessage(ABM_SETSTATE, &appbard);
				*/

			}
			GetDevice()->Reset(pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags);
			pConfig->SetProperty(GetDevice());
		}	
	}
	else
	{
		GetDevice()->Reset(pConfig->screenWidth,pConfig->screenHeight,pConfig->colorBits,dwFlags);
		pConfig->SetProperty(GetDevice());
	}
	
	
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hInstance : 描述
 * \param lpCmdLine : 描述
 * \param nCmdShow : 描述
 * \return 返回值的描述
 */
int __stdcall GameMain( HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow )
{
	if( !Engine_GetAppation()->InitInstance() )
		return -1;

	return Engine_GetAppation()->Run();
}
