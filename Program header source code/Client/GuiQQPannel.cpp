#include "public.h"
#include "./Game.h"
#include "../engine/include/engine.h"
#include "../gui/include/guiTypes.h"
#include "./GuiQQPannel.h"
#include "../xml_parse/XmlParseEx.h"
#include "./GuiAuctionDialog.h"

const static int	g_cPannelItemHeight = 30;
const static int	g_cPannelItemWidth	= 107;
const static int	g_cPannelChildItemHeight = 17;
const static int	g_cPannelChildItemWidht = 89;

const static stPointI g_cLeftTopIncisePt(5,5);
const static stPointI g_cBottomRightIncisePt(5,5);

void stPannelItem::Parse(TiXmlElement* Item)
{
	FUNCTION_BEGIN;
	if( NULL == Item) return;
	TiXmlElement* Type = Item->FirstChildElement("Type");
	while( Type )
	{
		int type = 0;
		if( Type->QueryIntAttribute("type",&type) == TIXML_SUCCESS )
		{
			m_listType.push_back((enumItemType)(type));
		}		
		Type = Type->NextSiblingElement("Type");
	}
	FUNCTION_END;
}

bool stPannelItem::bIsItemTypeIn(enumItemType type)
{
	FUNCTION_BEGIN;
	for ( int i = 0; i < m_listType.size(); ++i )
	{
		if ( type == m_listType[i] )
		{
			return true;
		}
	}
	return false;
	FUNCTION_END;
}

bool stPannelItem::OnClick(enumPannelType type)
{
	FUNCTION_BEGIN;
	if( type == ePannel_Browse )
	{
		m_pButton->SetChecked(true);
		return ((CGuiAuctionDialog*)(m_pDialog))->OnBrowseSearch((BYTE)m_type);
	}
	if( type == ePannel_Roup )
	{
		return ((CGuiAuctionDialog*)(m_pDialog))->OnRoupSearch((BYTE)m_type);
	}

	//dnf
	return true;
	FUNCTION_END;
}

CPannelItem::CPannelItem()
{
	FUNCTION_BEGIN;
	m_pMasterBtn = NULL;
	m_pDialog = NULL;
	FUNCTION_END;
}

CPannelItem::~CPannelItem()
{
	FUNCTION_BEGIN;
	Release();
	FUNCTION_END;
}

CPannelItem::CPannelItem(CGuiDialog* pDialog,CGuiButton* pMasterBtn)
{
	FUNCTION_BEGIN;
	m_pDialog = pDialog;
	m_pMasterBtn = pMasterBtn;
	FUNCTION_END;
}

stPannelItem* CPannelItem::AddPannelItem(int id,const char* pszText)
{
	FUNCTION_BEGIN;
	Assert(m_pDialog);
	stPannelItem* pItem = new stPannelItem();
	if( FAILED(m_pDialog->AddButton(id,NULL,0,0,0,0,0,0,&(pItem->m_pButton))) )
	{
		return NULL;
	}

	pItem->m_pButton->SetButtonType(CGuiButton::ButtonType_Bitmap);
	pItem->SetBitmap(stResourceLocation("data\\Interfaces.gl",55,34),3);	//default bitmap
	pItem->SetText(pszText);
	pItem->SetVisible(false);
	pItem->m_pDialog = m_pDialog;
	m_pDialog->SetButtonGroup(id,1);
	pItem->m_pButton->SetTextColor(-1);
	m_list.push_back(pItem);
	return pItem;
	FUNCTION_END;
}

void CPannelItem::SetButtonLocation(int id,const POINT& pt)
{
	FUNCTION_BEGIN;
	SetButtonLocation(id,pt.x,pt.y);
	FUNCTION_END;
}

void CPannelItem::SetButtonLocation(int id,int x,int y)
{
	FUNCTION_BEGIN;
	tListPannelItemIt it = GetButtonById(id);
	if( it != m_list.end() )
	{
		(*it)->SetLocation(x,y);
	}
	FUNCTION_END;
}

void CPannelItem::SetButtonBitmap(stResourceLocation& rl,int frameNum,int id )
{
	FUNCTION_BEGIN;
	for( tListPannelItemIt it = m_list.begin(); it != m_list.end(); ++it )
	{	
		if( id != 0 && (*it)->GetID() != id )
		{
			continue;
		}
		(*it)->SetBitmap(rl,frameNum);
	}
	FUNCTION_END;
}

int	 CPannelItem::GetID()
{
	FUNCTION_BEGIN;
	if( NULL != m_pMasterBtn )
	{
		return m_pMasterBtn->GetID();
	}
	return 0;
	FUNCTION_END;
}

POINT CPannelItem::GetLocation()
{
	FUNCTION_BEGIN;
	if( NULL != m_pMasterBtn )
	{
		return m_pMasterBtn->GetLocation();
	}
	return stPointI(0,0);
	FUNCTION_END;
}

void CPannelItem::SetBitmap(stResourceLocation& rl,int frameNum)
{
	FUNCTION_BEGIN;
	if( NULL != m_pMasterBtn )
	{
		return m_pMasterBtn->SetBitmap(rl,frameNum);
	}
	FUNCTION_END;
}

void CPannelItem::SetLocation(int x,int y)
{
	FUNCTION_BEGIN;
	Assert(m_pMasterBtn);
	m_pMasterBtn->SetLocation(x,y);
	int xPixel = x + ( g_cPannelItemWidth - g_cPannelChildItemWidht )/2;
	int yPixel = y + g_cPannelItemHeight;
	for( int i = 0; i< m_list.size(); ++i )
	{
		m_list[i]->SetLocation(xPixel,yPixel + g_cPannelChildItemHeight * i );
	}	
	FUNCTION_END;
}

void CPannelItem::SetChecked( bool bChecked )
{
	FUNCTION_BEGIN;
	if( NULL != m_pMasterBtn )
	{
		m_pMasterBtn->SetChecked(bChecked);
		if( bChecked )
		{
			m_pMasterBtn->SetTextColor(0xff00ff00);
		}
		else
		{
			m_pMasterBtn->SetTextColor(-1);
		}
	}
	SetAllButtonVisible(bChecked);
	FUNCTION_END;
}

bool CPannelItem::GetChecked()
{
	FUNCTION_BEGIN;
	Assert(m_pMasterBtn);
	if( NULL != m_pMasterBtn )
	{
		return m_pMasterBtn->GetChecked();
	}
	return false;
	FUNCTION_END;
}

tListPannelItemIt CPannelItem::GetButtonById(int id)
{
	FUNCTION_BEGIN;
	for( tListPannelItemIt it = m_list.begin(); it != m_list.end(); ++it )
	{
		if( (*it)->GetID() == id )
		{
			return it;
		}
	}
	return m_list.end();
	FUNCTION_END;
}

void CPannelItem::SetAllButtonVisible(bool bVisible)
{
	FUNCTION_BEGIN;
	for( tListPannelItemIt it = m_list.begin(); it != m_list.end(); ++it )
	{
		(*it)->SetVisible(bVisible);
	}
	FUNCTION_END;
}

void CPannelItem::AddToTab(CGuiTab* &pTab,int index)
{
	FUNCTION_BEGIN;
	for( tListPannelItemIt it = m_list.begin(); it != m_list.end(); ++it )
	{
		(*it)->SetVisible(false);
		pTab->AddControl( index,(*it)->m_pButton );
	}
	pTab->AddControl(index,m_pMasterBtn);
	FUNCTION_END;
}

void CPannelItem::Parse(TiXmlElement* pannel)
{
	FUNCTION_BEGIN;
	if( NULL == pannel) return;
	TiXmlElement* item = pannel->FirstChildElement("Item");
	while( item )
	{
		int id = 0;
		if( item->QueryIntAttribute("id",&id) != TIXML_SUCCESS )
		{
			id = 0;
		}
		int type = 0;
		if( item->QueryIntAttribute("type",&type) != TIXML_SUCCESS )
		{
			type = 0;
		}
		const char* Text = item->Attribute("Text");
		if( NULL != Text && strlen(Text) > 0 )
		{
			stPannelItem* pItem = AddPannelItem(id,Text);
			if ( NULL != pItem )
			{
				pItem->Parse( item );
				pItem->m_type = type;
			}
		}
		item = item->NextSiblingElement("Item");
	}
	FUNCTION_END;
}

void CPannelItem::Release()
{
	FUNCTION_BEGIN;
	FreePointerList<tListPannelItem>(m_list);
	FUNCTION_END;
}

void CPannelItem::SetVisible(bool bShow)
{
	FUNCTION_BEGIN;
	for( tListPannelItemIt it = m_list.begin(); it != m_list.end(); ++it )
	{
		(*it)->SetVisible(bShow);
	}
	if( m_pMasterBtn )
	{
		m_pMasterBtn->SetVisible(bShow);
	}
	FUNCTION_END;
}


/*************************************************************************/

CGuiQQPannel::CGuiQQPannel()
{
	FUNCTION_BEGIN;
	m_pDialog = NULL;
	FUNCTION_END;
}

CGuiQQPannel::CGuiQQPannel(CGuiDialog* pDialog)
{
	FUNCTION_BEGIN;
	SetBorderBitmap(stResourceLocation("data\\Interfaces.gl",55,37));
	SetDialog(pDialog);
	FUNCTION_END;
}

CGuiQQPannel::~CGuiQQPannel()
{
	FUNCTION_BEGIN;
	Release();
	FUNCTION_END;
}

CPannelItem* CGuiQQPannel::AddPannel(int id,const char* szText)
{
	FUNCTION_BEGIN;
	Assert(m_pDialog);
	CGuiButton* pBtn = NULL;
	if( FAILED(m_pDialog->AddButton(id,NULL,0,0,0,0,0,0,&(pBtn))) )
	{
		return NULL;
	}
	pBtn->SetButtonType(CGuiButton::ButtonType_Bitmap);
	pBtn->SetBitmap(stResourceLocation("data\\Interfaces.gl",55,31),3);	//default bitmap
	pBtn->SetText(szText);
	CPannelItem* pPannel = new CPannelItem(m_pDialog,pBtn);
	m_listPannel.push_back(pPannel);
	return pPannel;
	FUNCTION_END;
}

void CGuiQQPannel::AddChildItem(int pannelid,int id,const char* szText)
{
	FUNCTION_BEGIN;
	tListPannelIt it = GetPannelById( pannelid );
	if( it != m_listPannel.end() )
	{
		(*it)->AddPannelItem(id,szText);
	}
	FUNCTION_END;
}

tListPannelIt CGuiQQPannel::GetPannelById(int id)
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{
		if( (*it)->GetID() == id )
		{
			return it;
		}
	}
	return m_listPannel.end();
	FUNCTION_END;
}

CPannelItem* CGuiQQPannel::GetPannelByIndex(int index)
{
	FUNCTION_BEGIN;
	if( index >= 0 && index < m_listPannel.size() )
	{
		return m_listPannel[index];
	}
	return NULL;
	FUNCTION_END;
}


void CGuiQQPannel::Release()
{
	FUNCTION_BEGIN;
	FreePointerList<tListPannel>(m_listPannel);
	FUNCTION_END;
}

void CGuiQQPannel::SetPannelBitmap(stResourceLocation& rl,int frameNum,int id )
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{	
		if( id != -1 && (*it)->GetID() != id )
		{
			continue;
		}
		(*it)->SetBitmap(rl,frameNum);
	}
	FUNCTION_END;
}

void CGuiQQPannel::SetBorderBitmap(const stResourceLocation& rl)
{
	FUNCTION_BEGIN;
	m_pBitmapBorder = GetDevice()->FindBitmaps( &rl );
	if ( NULL != m_pBitmapBorder )
	{
		IBitmap* pBitmap = m_pBitmapBorder->GetBitmap(0);
		if ( pBitmap )
		{
			stRectI rcTemp = pBitmap->GetClipRect();
			CuttRect9(rcTemp,g_cLeftTopIncisePt,g_cBottomRightIncisePt,m_rcBitmapClip);			
		}		
	}	
	FUNCTION_END;
}

void CGuiQQPannel::UpdateBorderRects()
{
	FUNCTION_BEGIN;
	CuttRect9(m_BorderRect,g_cLeftTopIncisePt,g_cBottomRightIncisePt,m_rcRender);
	UpdatePannelLocation();
	FUNCTION_END;
}

bool CGuiQQPannel::OnButtonClick(int id)
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{
		//如果点的是大按钮
		if( (*it)->GetID() == id )
		{
			SetPannelItemChecked(id);
			tListPannelItemIt ItemIt = (*it)->m_list.begin();
			if ( ItemIt != (*it)->m_list.end() )
			{
				(*ItemIt)->OnClick( m_type );
			}
			return true;
		}
		//如果点的是小按钮
		tListPannelItemIt ItemIt = (*it)->GetButtonById( id );
		if ( ItemIt != (*it)->m_list.end() )
		{
			return (*ItemIt)->OnClick( m_type );
		}
	}
	return false;
	FUNCTION_END;
}

tListPannelIt CGuiQQPannel::GetCheckedIt()
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{
		if( true == (*it)->GetChecked() )
		{
			return it;
		}
	}
	return m_listPannel.end();
	FUNCTION_END;
}

void CGuiQQPannel::ShowPannelItem(int index,bool bShow)
{
	FUNCTION_BEGIN;
	CPannelItem* pItem = GetPannelByIndex(index);
	if ( NULL != pItem)
	{
		pItem->SetAllButtonVisible(bShow);
	}
	FUNCTION_END;
}

void CGuiQQPannel::SetVisible(bool bShow)
{
	FUNCTION_BEGIN;
	if( bShow )
	{
		SetPannelItemChecked(0);
		return ;
	}
	for( int i = 0; i < m_listPannel.size(); ++i )
	{
		m_listPannel[i]->SetVisible(false);
	}
	FUNCTION_END;
}

void CGuiQQPannel::SetCurrentPannel(int index)
{
	FUNCTION_BEGIN;
	for( int i = 0; i < m_listPannel.size(); ++i )
	{
		if( index != i )
		{
			m_listPannel[i]->SetChecked(false);
		}
		else
		{
			m_listPannel[i]->SetChecked(true);
		}
	}
	UpdatePannelLocation();	
	FUNCTION_END;
}

void CGuiQQPannel::SetPannelItemChecked(int id)
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{
		if( id == (*it)->GetID() )
		{
			(*it)->SetChecked(true);
		}
		else
		{
			(*it)->SetChecked(false);
		}
	}
	UpdatePannelLocation();	
	FUNCTION_END;
}

void CGuiQQPannel::UpdatePannelLocation()
{
	FUNCTION_BEGIN;
	int size = m_listPannel.size();

	int iXPixel = m_BorderRect.left;
	int iYPixel = m_BorderRect.top;
	for( int i = 0; i < size; ++i )
	{
		m_listPannel[i]->SetLocation(iXPixel,iYPixel);
		if( m_listPannel[i]->GetChecked() )
		{
			iYPixel = m_BorderRect.bottom - (size - i )*g_cPannelItemHeight;
		}
		else
		{
			m_listPannel[i]->SetChecked(false);
		}
		iYPixel += g_cPannelItemHeight;
	}
	FUNCTION_END;
}

void CGuiQQPannel::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	//只显示边框,0代表中间的区域
	for(size_t i = 1;i < 9; ++i)
	{
		stRectI rc = m_rcRender[i];
		rc.OffsetRect( m_pDialog->GetLocation() );		
		PatchRenderImage(rc,m_pBitmapBorder,&m_rcBitmapClip[i],-1,Blend_Null,s_PatchAlignFormat[i]);
	}
	FUNCTION_END;
}

void CGuiQQPannel::AddToTab(CGuiTab* &pTab,int index)
{
	FUNCTION_BEGIN;
	for( tListPannelIt it = m_listPannel.begin(); it != m_listPannel.end(); ++it )
	{
		(*it)->AddToTab(pTab,index);
	}
	FUNCTION_END;
}

bool CGuiQQPannel::Load(const char* pszFileName)
{
	FUNCTION_BEGIN;
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak",pszFileName);
	if(!pStream) return false;	
	if(	true == LoadXMLFromStream(pStream,doc) )
	{
		TiXmlElement* QQPannel = doc.FirstChildElement("QQPannel");
		if( QQPannel )
		{
			TiXmlElement* pannel = QQPannel->FirstChildElement("Pannel");
			while( pannel )
			{
				int id = 0;
				if( pannel->QueryIntAttribute("id",&id) != TIXML_SUCCESS )
				{
					id = 0;
				}
				const char* Text = pannel->Attribute("Text");
				if( NULL != Text && strlen(Text) > 0 )
				{
					CPannelItem* pPannel = AddPannel(id,Text);
					if ( pPannel )
					{
						pPannel->Parse(pannel);
					}
				}	
				pannel = pannel->NextSiblingElement("Pannel");
			}
		}
	}
	ClosePackFileForRead(pStream);
	return true;
	FUNCTION_END;
}
