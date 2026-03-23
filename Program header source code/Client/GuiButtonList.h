#pragma once
#include "..\gui\include\GuiButton.h"

enum enmBtnListType{
	BTN_LIST_TYPE_UP = 0,
	BTN_LIST_TYPE_DOWN = 1,
};

typedef std::vector<CGuiButton*> btnList;
class CGuiButtonList
{
	btnList m_aList;

	enmBtnListType m_type;
	int m_space;
	stPointI m_pos;
	CGuiButton* m_pActiveBtn;
	bool m_bPop;


public:
	CGuiButtonList();
	~CGuiButtonList();

	void Clear();

	bool IsPop() { return m_bPop; }

	void SetType(enmBtnListType type) { m_type = type; }
	enmBtnListType GetType() { return m_type; }

	void SetSpace(int space) { m_space = space; }
	int GetSpace() { return m_space; }

	void SetLocation(stPointI pos) { m_pos = pos; }
	stPointI GetLocation() { return m_pos; }

	void AddOneButton( CGuiButton* pBtn );
	void DeleteOneButton( CGuiButton* pBtn );

	int GetBtnNum() { return m_aList.size(); }

	CGuiButton* GetActiveBtn() { return m_pActiveBtn; }
	void SetActiveBtn( CGuiButton* pBtn );

	void PopBtnList();
	void TurnOffBtnList();
	void ResetBtnLocation();

	void OnBtnClickEvent( CGuiButton* pBtn );

	void OnRender(float fElapsedTime);
};