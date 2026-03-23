/**
 *\file		GuiItemDetailInfo.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:14:00 中国标准时间
 *\brief	道具详细信息界面
 *
 * 
 *
*/

#include "public.h"
#include "../gui/include/guiTypes.h"
#include "./GameGuiManager.h"
#include "./GuiPrivateChat.h"
#include ".\guiitemdetailinfo.h"
#include "./GuiChatOutput.h"
#include "./command.h"
#include "./Game.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./RoleItem.h"
#include "../xml_parse/XmlParseEx.h"

///道具详细信息
//static CToolTips infoDetail[ITEM_DETAIL_INFO_COUNT];

/**
 * \brief 构造函数
 * 
 *	
 * 
 * \return 
 */
CGuiItemDetailInfo::CGuiItemDetailInfo(void)
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiItemDetailInfo::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	
	if ( GetCloseButton() )
		GetCloseButton()->SetVisible( false );

	m_aItemDetailList.clear();
	
	OnResetScreenSize();

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiItemDetailInfo::OnClose()
{
	FUNCTION_BEGIN;

	m_aItemDetailList.clear();

	GetGameGuiManager()->m_guiItemDetailInfo = NULL;

	FUNCTION_END;
}

/**
 * \brief 对话框控件消息处理
 * 
 * 详细描述
 * 
 * \param nEvent : 
 * \param nID : 
 * \param pControl : 
 * \return 
 */
bool CGuiItemDetailInfo::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_DIALOG_COMMAND:
		break;
	case EVENT_BUTTON_CLICKED:
		switch( nID )
		{
		case 20:			//exit
			Close();
			break;
		case 102:			//into private chat
			Close();
			break;
		case 2:
		case 3:
		case 4:
			//Get corresponding item
			if ( !m_aItemDetailList.empty() )
			{
				//if the last btn is clicked we close all
				if ( (nID - 2) == m_aItemDetailList.size()-1 )
				{
					Close();
					break;
				}

				aItemDetailInfoIt itor;
				//First we clear all item tips
				for( itor=m_aItemDetailList.begin(); itor!=m_aItemDetailList.end(); itor++ )
					(*itor).infoDetail.Clear();

				itor = m_aItemDetailList.begin();
				for( int i=0;i<nID-2;i++ )
				{
					if ( i< ITEM_DETAIL_INFO_COUNT )
					{
						itor++ ;
					}
				}

				if ( itor != m_aItemDetailList.end() )
				{
					m_aItemDetailList.erase( itor );
					UpdateItemDetailInfo();
				}
			}

			if ( m_aItemDetailList.empty() )
				Close();

			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return CGuiDialog::OnGuiEvent( nEvent,nID, pControl );

	FUNCTION_END;
}

/**
 * \brief 绘制对话框背景
 * 
 * 
 * 
 * \param fElapsedTime : 
 * \return 
 */
void CGuiItemDetailInfo::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	//CGuiDialog::RenderBackground( fElapsedTime );
	
	FUNCTION_END;
}

/**
 * \brief 绘制对话框
 * 
 *		
 * 
 * \para	m fElapsedTime : 
 * \return 
 */
HRESULT CGuiItemDetailInfo::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	//HRESULT hr;
	//hr = CGuiDialog::OnRender( fElapsedTime );

	//Get info pos
	int i=0;
	for( aItemDetailInfoIt it=m_aItemDetailList.begin(); it!=m_aItemDetailList.end(); it++ )
	{
		//Render detail
		(*it).Render( m_x, m_y );

		//Render btn
		if ( GetButton( i+2 ) )
			GetButton( i+2 )->Render( fElapsedTime );

		i++ ;
	}

	/*if ( GetButton( 2 ) )
		GetButton( 2 )->Render( fElapsedTime );*/
	
	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 设置新的道具
 * 
 * 
 * 
 * \param itemId : 道具id
 * \return 
 */
void CGuiItemDetailInfo::SetActiveItem(  const t_Object& object, const char* ownerName )
{
	FUNCTION_BEGIN;

	//See if we already has this item in list
	/*for( aItemDetailInfoIt itor=m_aItemDetailList.begin(); itor!=m_aItemDetailList.end(); itor++ )
	{
		if ( (*itor).object.qwThisID == object.qwThisID )
			return;
	}*/

	//Add this item to our list
	if ( m_aItemDetailList.size() >= 3 )
		m_aItemDetailList.pop_back();

	m_aItemDetailList.push_front( stExchangeItem(object, ownerName) );
	UpdateItemDetailInfo();

	FUNCTION_END;
}

void CGuiItemDetailInfo::OnResetScreenSize()
{
	if ( GetGameGuiManager()->m_guiChatOutput )		//
	{
		SetLocation( GetGameGuiManager()->m_guiChatOutput->GetLocation().x + GetGameGuiManager()->m_guiChatOutput->GetWidth() + 2,
			GetGameGuiManager()->m_guiChatOutput->GetLocation().y );
	}
	else
	{
		SetLocation( 150, GetDevice()->GetHeight()-300 );
	}
}

/**
 * \brief 刷新道具详细信息
 * 
 * 
 * 
 *	\return 
 */
void CGuiItemDetailInfo::UpdateItemDetailInfo()
{
	FUNCTION_BEGIN;

	if ( m_aItemDetailList.empty() )
		return;

	//SetLocation( (sceneWidth-width)/2, (sceneHeight-height)/2-50 );
	if ( GetGameGuiManager()->m_guiChatOutput )		//
	{
		SetLocation( GetGameGuiManager()->m_guiChatOutput->GetLocation().x + GetGameGuiManager()->m_guiChatOutput->GetWidth() + 2,
			GetGameGuiManager()->m_guiChatOutput->GetLocation().y );
	}
	else
	{
		SetLocation( 150, GetDevice()->GetHeight()-300 );
	}

	//Get item detail info in list
	int width = 0;
	int height = 0;
	
	//hide all close btn
	for( int k=0;k<ITEM_DETAIL_INFO_COUNT;k++ )
	{
		if ( GetButton( k+2 ) )
			GetButton( k+2 )->SetVisible( false );
	}

	int i = 0;
	for( aItemDetailInfoIt it=m_aItemDetailList.begin(); it!=m_aItemDetailList.end(); it++ )
	{
		//infoDetail.SetBorderTexVisible( false );
		//infoDetail.SetBkColor( D3DCOLOR_ARGB(200,0,0,0) );
		//infoDetail.SetUseCustomeBkColor( true );
		//infoDetail.SetBkColor( D3DCOLOR_ARGB(0,0,0,0) );
		ObjectBase_t* objectBase = GetObjectBase( (*it).object.dwObjectID );
		if ( NULL == objectBase )
			continue;
		
		(*it).infoDetail.Clear();
		UpdateMyObjectToolTips( (*it).object, objectBase, (*it).infoDetail, 0 );
		
		(*it).ptInfo = stPointI(width, 0);

		width += (*it).infoDetail.GetWidth();
		if ( height < (*it).infoDetail.GetHeight() )
			height = (*it).infoDetail.GetHeight();

		//Set close btn pos
		if ( GetButton( i+2 ) )
		{
			GetButton( i+2 )->SetLocation( width-96, -71 );
			GetButton( i+2 )->SetVisible( true );
		}

		i++ ;
	}

	//Set dlg size
	SetSize( width, height );

	/*if ( GetButton( 2 ) )
		GetButton( 2 )->SetLocation( m_width-96, -71 );*/
	
	FUNCTION_END;
}




