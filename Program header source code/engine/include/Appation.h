#pragma once

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IDevice;
struct stConfigBase
{
	int screenWidth;
	int screenHeight;
	bool bWindow;				// 窗口模式
	bool bVertSync;				// 垂直同步
	bool bLightMap;				// 光影
	bool bWeather;				// 天气效果
	bool bMultiTex;				// 多层纹理
	bool bRenderTarget;			// 
	bool bUseDXT;				// 使用压缩纹理格式
	bool bHardwareCursor;	// 是否使用硬件鼠标

	bool bMainSound;			//主音效开关
	bool bSceneSound;			//场景音效开关
	bool bCircumstanceSound;	//环境音效开关
	bool bBackgroundSound;		//背景音效开关

	int nMainSound;				//主音效音量
	int nSceneSound;			//场景音效音量
	int nCircumstanceSound;		//环境音效音量
	int nBackgroundSound;		//背景音效音量

	int colorBits;	// 颜色位数
	DWORD GetFlags();
	stConfigBase();
	void SetFlags(DWORD);
	void SetProperty(IDevice*);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CAppation
{
public:

	HWND	m_hWnd;
	char	m_szWndClassName[64];
	char	m_szWndName[64];
	char	m_szAppPath[MAX_PATH];
	HICON	m_hIcon;
	HINSTANCE m_hInstance;
	enumProgramType m_eProgramType;
	DWORD	m_dwThreadID;
public:
	CAppation(void);
	~CAppation(void);
	virtual bool AforeDraw(); 
	virtual void BeforeDraw();
	virtual void Draw(float fElapsedTime);
	virtual void RefreshFrame(float fElapsedTime);
	virtual bool InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	virtual LRESULT MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual stConfigBase* GetConfigBase() = 0;
	virtual void LoadConfig() = 0;
	virtual void SaveConfig() = 0;
	virtual void Exit() = 0;
	virtual void ApplyConfig();
	HWND GetWnd() { return m_hWnd; }
	const char*  GetAppPath(){ return m_szAppPath;}
	virtual bool ConverPicChar(const char* str,WORD& ch,int & charNum){ return false;}
	virtual bool SearchPicChar(char* str,char* strContext,const char* strSeparator,int index){ return false;}
	virtual int  GetPicCount(){ return 0;}
};
extern int __stdcall GameMain( HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow );