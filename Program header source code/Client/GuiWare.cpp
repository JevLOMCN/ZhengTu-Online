#include "public.h"

#include "GuiWare.h"

CGuiWare::CGuiWare(CGuiImage* pImg, CGuiButton* pBtn, CGuiStatic* pSta1, CGuiStatic* pSta2, CGuiTable* pTable)
{
	m_pImage = pImg;
	m_pButton = pBtn;
	m_pStatic1 = pSta1;
	m_pStatic2 = pSta2;
	m_pTable = pTable;
}

CGuiWare::~CGuiWare()
{

}

void CGuiWare::Show(bool bShow)
{
	if (m_pImage && m_pButton && m_pStatic1 && m_pStatic2 && m_pTable)
	{
		m_pImage->SetVisible(bShow);
		m_pButton->SetVisible(bShow);
		m_pStatic1->SetVisible(bShow);
		m_pStatic2->SetVisible(bShow);
		m_pTable->SetVisible(bShow);
	}
}

void CGuiWare::SetPrice(const std::string& strPrice)
{
	if (m_pStatic2)
	{
		m_pStatic2->SetText(strPrice.c_str());
	}
}

void CGuiWare::SetName(const std::string& strName)
{
	if (m_pStatic1)
	{
		m_pStatic1->SetText(strName.c_str());
	}
}
