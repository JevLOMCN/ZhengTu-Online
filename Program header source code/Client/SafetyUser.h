#pragma once 


class CGuiOpenSafetyDlg;
class CGuiOptionSafetyDlg;
class CGuiNumdDlg;


enum eNumDlg_Usage
{
	eUsage_OpenSafety,
	eUsage_OptionSafety,
	eUsage_TmpClose,
};

class CSafetyUser 
{
public:
	explicit CSafetyUser();
	~CSafetyUser();

	void Reset();

private:

protected:
	static CSafetyUser* Singleton;
public:
	static CSafetyUser* Instance();


    bool IsStateClose()
	{
		return m_state == (BYTE)-1;
	}

	bool IsStateTmpClose()
	{
		return SAFETY_TEMP_CLOSE == m_state;
	}

    bool IsStateSafetyOpen()
	{
		//return isset_state(&m_state,SAFETY_OPEN);
		return SAFETY_OPEN == m_state;
	}

	//enum
	//{
	//	SAFE_SPLIT_OBJECT, // 拆装备时,需要验证
	//	SAFE_THIRD_PACK,   // 从第三个包裹中拿出物品,需要验证
	//	SAFE_GOLD_STOCK,   // 金币购买股票
	//};
	bool IsSafeDecomposeObject()
	{
		if( !IsStateSafetyOpen())
			return false;
		return isset_state(&m_safesetup,SAFE_SPLIT_OBJECT);
	}

	bool IsSafe23Pack()
	{
		if( !IsStateSafetyOpen())
			return false;
		 return isset_state(&m_safesetup,SAFE_THIRD_PACK);
	}

	bool IsSafeGoldStock()
	{
		if( !IsStateSafetyOpen())
			return false;
		return isset_state(&m_safesetup,SAFE_GOLD_STOCK);
	}

public:
	void SetStateInfo(BYTE state,BYTE safesetup);

private:
	BYTE m_state;
	BYTE m_safesetup;

public:
	void  OnBtnClick();

	void OpenSafety();

	CGuiOptionSafetyDlg*AddOptionSafeDlg();
	CGuiNumdDlg*AddNumDlg(const eNumDlg_Usage & usage);


	CGuiOptionSafetyDlg * m_guiOptionSafetyDlg;
	CGuiNumdDlg         * m_guiNumDlg;


private:

	void OptionSafety();
	
};


inline  
CSafetyUser* CSafetyUser::Instance()
{
	Assert(Singleton != 0);
	return Singleton;
}




/////////////////////////////////////////////////////////////
extern bool ParseSafetyMessage(stNullUserCmd* pCmd,size_t size);


////////////////////////////////////////////////////////////////
class CGuiOptionSafetyDlg :
	public  CGuiDialog 
{
public:
	CGuiOptionSafetyDlg();
	~CGuiOptionSafetyDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	void FinishTempClose();
	void FinishOptionSafety();

	DWORD m_dwPass;

};


class CGuiNumdDlg :
	public CGuiDialog
{
public:	
	CGuiNumdDlg( const eNumDlg_Usage & usage );

	void OnCreate();
	void OnClose();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
private:

	eNumDlg_Usage  m_Usage;

	std::string m_strNum;
};

