#pragma once

class CGuiButton;
class CGuiStatic;
class CGuiTable;
class CGuiWare
{
public:
	CGuiWare(CGuiImage* pImg, CGuiButton* pBtn, CGuiStatic* pSta1, CGuiStatic* pSta2, CGuiTable* pTable);
	~CGuiWare();

	void Show(bool bShow);
	void SetPrice(const std::string& strPrice);
	void SetName(const std::string& strName);

public:
	CGuiTable*  m_pTable;

private:
	CGuiImage* m_pImage;
	CGuiButton* m_pButton;
	CGuiStatic* m_pStatic1;
	CGuiStatic* m_pStatic2;
	
};
