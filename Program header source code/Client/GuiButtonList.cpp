#include "public.h"
#include "../gui/include/guiTypes.h"
#include "./GuiButtonList.h"

//Construct
CGuiButtonList::CGuiButtonList()
{
	m_type = BTN_LIST_TYPE_UP;
	m_aList.clear();
	m_pos.x = 0;
	m_pos.y = 0;

	m_space = 0;
	m_bPop = false;

	m_pActiveBtn = NULL;
}

//Destruction
CGuiButtonList::~CGuiButtonList()
{
	Clear();
}

//Clear list
void CGuiButtonList::Clear()
{
	for( btnList::iterator it = m_aList.begin(); it != m_aList.end(); it++ )
	{
		(*it) = NULL;
	}
}

//Add One Btn to list
void CGuiButtonList::AddOneButton( CGuiButton* pBtn )
{
	if ( NULL == pBtn )
		return;

	if ( m_aList.empty() )
		m_pActiveBtn = pBtn;
	
	if ( pBtn == m_pActiveBtn )
		pBtn->SetVisible( true );
	else
		pBtn->SetVisible( false );

	m_aList.push_back( pBtn );
}

//Delete one btn from list
void CGuiButtonList::DeleteOneButton( CGuiButton* pBtn )
{
	//TBD
}

//Set active btn
void CGuiButtonList::SetActiveBtn( CGuiButton* pBtn )
{
	if ( NULL == pBtn )
		return;

	if ( m_aList.empty() )
	{
		m_pActiveBtn = pBtn;
		return;
	}

	m_pActiveBtn = pBtn;

	TurnOffBtnList();
}

//Reset all btn location
void CGuiButtonList::ResetBtnLocation()
{
	if ( m_pActiveBtn )
		m_pActiveBtn->SetLocation( m_pos.x, m_pos.y );

	int x = m_pos.x;
	int y = m_pos.y;
	int space = m_space;
	switch( m_type )
	{
	case BTN_LIST_TYPE_UP:
		space *= -1;
		break;
	case BTN_LIST_TYPE_DOWN:
		space *= 1;
		break;
	}

	for( btnList::iterator it = m_aList.begin(); it != m_aList.end(); it++ )
	{
		if ( (*it) != m_pActiveBtn )
		{
			y += space;
			(*it)->SetLocation( x, y );
		}
	}
}

//Pop btn list
void CGuiButtonList::PopBtnList()
{
	for( btnList::iterator it = m_aList.begin(); it != m_aList.end(); it++ )
		(*it)->SetVisible( true );
	
	m_bPop = true;
	ResetBtnLocation();
}

//Turn off btn list
void CGuiButtonList::TurnOffBtnList()
{
	for( btnList::iterator it = m_aList.begin(); it != m_aList.end(); it++ )
	{
		if ( (*it) != m_pActiveBtn )
			(*it)->SetVisible( false );
		else
			(*it)->SetVisible(true);
	}
	m_bPop = false;

	ResetBtnLocation();
}

//On one btn click event
void CGuiButtonList::OnBtnClickEvent( CGuiButton* pBtn )
{
	if ( NULL == pBtn )
		return;

	if ( pBtn == m_pActiveBtn )
	{
		if ( m_bPop )
			TurnOffBtnList();
		else
			PopBtnList();
	}
	else
	{
		m_pActiveBtn = pBtn;
		TurnOffBtnList();
	}
}

//Render btn list
void CGuiButtonList::OnRender( float fElapsedTime )
{
	for( btnList::iterator it = m_aList.begin(); it != m_aList.end(); it++ )
	{
		if ( (*it)->IsVisible() )
			(*it)->Render( fElapsedTime );
	}
}
