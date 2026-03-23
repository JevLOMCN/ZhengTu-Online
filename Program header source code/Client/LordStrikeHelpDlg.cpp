#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include ".\lordstrikehelpDlg.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "../xml_parse/XmlParseEx.h"

//#define _M_DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////////
#define ID_MLTEXTBOX     1
#define ID_BUTTON_OK     2


CGuiGameHelpDlg::CGuiGameHelpDlg()
{
	m_bSmDialog = TRUE;
}

CGuiGameHelpDlg::~CGuiGameHelpDlg()
{

}

void CGuiGameHelpDlg::OnCreate(void)
{
	CGuiDialog::OnCreate();

	m_x = (GetDevice()->GetWidth() - GetWidth())/2;
	m_y = (GetDevice()->GetHeight() - GetHeight())/2;

	LoadDescFromFile();
}

void CGuiGameHelpDlg::LoadDescFromFile()
{
	CGuiMLTextBox * pMLTExtBox = GetMLTextBox(ID_MLTEXTBOX);
	if(!pMLTExtBox) 
		return;

	std::string strFileName = "datas\\lordstrike.xml";
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak",strFileName.c_str());
	if(!pStream)
	{
		Engine_WriteLog("×°ÔØ lordstrike.xml Ê§°Ü\n");
		return;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("½âÎö lordstrike.xml Ê§°Ü\n");
		return;
	}
	TiXmlElement* ddz = doc.FirstChildElement("ddz");
	if(ddz)
	{
		TiXmlElement* Text = ddz->FirstChildElement("Text");
		while(Text)
		{
			const char* pword = Text->Attribute("word");
			if( pword  )
			{
				pMLTExtBox->SetText(pword);
				break;
			}
		}
	}
	ClosePackFileForRead(pStream);
}

 
bool CGuiGameHelpDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case ID_BUTTON_OK:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiGameHelpDlg::OnClose(void)
{	 
        CLordStrikeManager::GetSingleton()->m_pGameHelpDlg = NULL;
}
