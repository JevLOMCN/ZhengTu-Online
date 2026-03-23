/**
 *\file		GuiFloatAccessKey.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:12:00 中国标准时间
 *\brief	浮动快捷键界面
 *
 * 
 *
*/

#include "public.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/RenderD3D.h"
#include "../gui/include/guiTypes.h"
#include ".\guimain.h"
#include ".\gameguimanager.h"
#include "./GameCursor.h"
#include "Game.h"
#include "game_script.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GameState.h"
#include "./GuiSkill.h"
#include "./GuiTeam.h"
#include "./RoleItem.h"
#include "./GuiFloatControl.h"
#include "./GuiFloatAccessKey.h"
#include "./GuiOtherAccessKey.h"
#include "./GameAccelKeyManager.h"
#include "./GuiChatSelect.h"
#include "./GuiEmotionDialog.h"
#include "../luabind/public.h"
#include "./GuiPetCtrlDialog.h"
#include "./GuiUserInfo.h"
int g_nFloatAccessBtnLeft = 5;
int g_nFloatAccessBtnTop = 5;	
int g_FloatAccessBtnSpace = 39;

DWORD dwState[] = { GUI_STATE_NORMAL,GUI_STATE_PRESSED,GUI_STATE_DISABLED };
/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CGuiFloatAccessKey::CGuiFloatAccessKey(void)
{
	FUNCTION_BEGIN;
	
	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
		m_pBtnAccessKey[i] = NULL;

	m_mouseOverAccessKeyNo = -1;

	m_activeSkillToolTips = NULL;

	m_bPretendedHide = false;

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::OnClose(void)
{
	FUNCTION_BEGIN;
	
	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		if ( m_pBtnAccessKey[i] )
			m_pBtnAccessKey[i] = NULL;
	}

	GetGameGuiManager()->m_guiFloatAcessKey = NULL;

	FUNCTION_END;
}

/**
 * \brief 创建对话框
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	

	//Get access key btn
	stPointF scale(0.8f, 0.8f);
	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		m_pBtnAccessKey[i] = GetButton( i+100 );
		if ( m_pBtnAccessKey[i])
		{
			if ( i != 0 )
				m_pBtnAccessKey[i]->SetLocation( g_nFloatAccessBtnLeft+(i-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
			else
				m_pBtnAccessKey[i]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
			
			m_pBtnAccessKey[i]->SetSize( 34, 34 );
			m_pBtnAccessKey[i]->SetBitmapScale( scale );
			//Set tooltips
			m_pBtnAccessKey[i]->GetToolTips()->Clear();
		}
	}
	
	//m_x = 422 + (GetDevice()->GetWidth() - 1024)/2;
	//m_y = 684 + (GetDevice()->GetHeight() - 768) - GetHeight();		//- GetHeight()		//671
	OnResetScreenSize();

	FUNCTION_END;
}

/**
 * \brief 重设对话框尺寸
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::OnResetScreenSize()
{
	if ( m_bPretendedHide )
	{
		m_x = -10000;
		m_y = -10000;
	}
	else
	{
		//Set pos follow chat select dlg
		if ( GetGameGuiManager()->m_guiChatSelect )
		{
			m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 422;
			m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 684 - GetHeight();
		}
		else
		{
			m_x = 422;
			m_y = 684 - GetHeight();
		}

		ResetDlgLocation();
	}

	BerthHide();
}

/**
* \brief 将第btnid个按钮的bitmapinfo设成rl.
*  
* \return 
*/
void CGuiFloatAccessKey::__SetEmotionAccessBtn(stResourceLocation & rl,int btnid)
{
	FUNCTION_BEGIN;

	/////////////////////////////////////////////////////////////////////////////////
	// Safe checking
	if( btnid < 0  || btnid >= FLOAT_ACCESS_KEY_NUM)
		return;

	/////////////////////////////////////////////////////////////////////////////////
	stPointI ptOffset(0,0);
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	stRectI rcClip(0,0,0,0);
	stRectI rcBitmap(0,0,0,0);
	if( pBmps)
	{
		IBitmap * pBitmap = pBmps->GetBitmap(0);
		if (pBitmap)
		{
			rcClip = pBitmap->GetClipRect();
			rcBitmap.right = pBitmap->GetWidth();
			rcBitmap.bottom = pBitmap->GetHeight();
		}
	}
	stPointF scale(1.0f,1.0f);
	if( rcClip.Width() > 34 || rcClip.Height() > 34)
	{
		scale.x = (float)34 / (float)rcClip.Width();
		scale.y = (float)34 / (float)rcClip.Height();  
		if( scale.x < scale.y )
			scale.y = scale.x;
		else
			scale.x = scale.y;
	}		
	ptOffset.x = (34 - rcClip.Width() * scale.x )/2   - rcClip.left * scale.x;
	ptOffset.y = (34 - rcClip.Height() * scale.y )/2  - rcClip.top * scale.y;
	if ( btnid != 0 )
		m_pBtnAccessKey[btnid]->SetLocation( g_nFloatAccessBtnLeft+(btnid-1)*g_FloatAccessBtnSpace+ptOffset.x, g_nFloatAccessBtnTop+ptOffset.y );
	else
		m_pBtnAccessKey[btnid]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace+ptOffset.x, g_nFloatAccessBtnTop + ptOffset.y);
	/////////////////////////////////////////////////////////////////////////////////
	m_pBtnAccessKey[btnid]->SetBitmapScale( scale );
	m_pBtnAccessKey[btnid]->SetBitmap( rl, 1 );

	FUNCTION_END;
}

/**
 * \brief 处理浮动快捷键点击事件
 * 
 *  脚本文件中调用
 * 
 * \param id : id
 * \return 
 */
inline void ParseFloatAccessBtnClickEvent( int id )
{
	FUNCTION_BEGIN;

	if ( GetGameGuiManager()->m_guiFloatAcessKey )
	{
		if ( (GetGameCursor()->IsDrag() && ((GetGameCursor()->GetCursorImageType() == eCursorSetAccess) || ( GetGameCursor()->GetCursorImageType() == eCursorDictate) ))  || GetGuiManager()->GetFloatItem() )
			GetGameGuiManager()->m_guiFloatAcessKey->SetAccessKey( id );
		else
		{
			if ( GetGameCursor()->IsDrag()
				&& (GetGameCursor()->GetCursorImageType() == eCursorEditAccess) )
			{
				if ( (id != -1)
					&& !(GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_FLOAT
						&& (GetGameAccelKeyManager()->m_curEditAccessKey.keyNo == id)) )		//Cur pos has access key btn
				{
					GetGameGuiManager()->m_guiFloatAcessKey->EditAccessKeyBtn( id );
				}
				else if( GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_FLOAT
						&& (GetGameAccelKeyManager()->m_curEditAccessKey.keyNo == id)
						&& ( (GetGameGuiManager()->m_guiFloatAcessKey->m_pBtnAccessKey[id]->GetDragOffset().x == 0)
							&& (GetGameGuiManager()->m_guiFloatAcessKey->m_pBtnAccessKey[id]->GetDragOffset().y == 0) ) )
				{
					RestoreMouseIcon();
				}
			}
			else
			{
				//Use access point item or skill
				enumGameAccelKeyType keyType;
				DWORD dwKeyID;
				BYTE curGroup = FLOAT_ACCESS_KEY_GROUP;
				BYTE key[4];
				key[0] = '0' + id;
				key[1] = 0;
				if(GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key))
				{
					switch( keyType )
					{
					case GameAccelKeyType_Skill:
						{
							if ( isset_state(GetScene()->GetMainCharacter()->GetState(),USTATE_CRAZY_ACTION)
								|| isset_state(GetScene()->GetMainCharacter()->GetState(),USTATE_CRAZY) )
							{
								RestoreMouseIcon();
								break;
							}
							//We call corresponding action according to skill target type
							SkillBase_t* pSelectSkill = GetSkillBase( dwKeyID );
							if ( pSelectSkill )
							{
								if ( GetScene()->GetMainCharacter()
									&& (GetScene()->GetMainCharacter()->GetSkillDisableStatus( pSelectSkill ) < 1) )
								{
									GetGameGuiManager()->AddClientSystemMessage( "技能处于冷却中!" );
									break;
								}

								if ( GetGameCursor()->IsDrag()
									&& (GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
								{
									if ( GetGameGuiManager()->m_guiFloatAcessKey )
										GetGameGuiManager()->m_guiFloatAcessKey->OnFindAccessSkill();
								}
								else
								{
									if ( GetGameCursor()->GetCursorImageType() == eCursorUseAccess )
									{
										if ( GetGameGuiManager()->m_guiTeam && GetGameGuiManager()->m_guiTeam->GetActiveSkill() )
										{
											//GetGameGuiManager()->m_guiTeam->UseSkillOnTeamMember( false );
											if ( pSelectSkill->dwID == GetGameGuiManager()->m_guiTeam->GetActiveSkill()->dwID )
												GetScene()->GetMainCharacter()->UseSkillOnCharacter( GetScene()->GetMainCharacter() );
										}
									}
									else
									{
										if ( IsCanSelectInHandSkill( pSelectSkill )
											&& ( !GetScene()->IsAnyAliveInterestNpc() /*|| (pSelectSkill->dwUseMethod & SkillUseMethod_BlueHand)*/ )
											/*&& !IsSelectFirstSkill( pSelectSkill )*/ )
										{
											//   My pet of type summon
											CPet* pPet = NULL;
											if ( pSelectSkill->dwID == SKILL_STRENGTHEN_SUMMON )
												pPet = GetScene()->FindPetByType( PET_TYPE_SUMMON );

											if ( pPet )
											{
												if ( GetGameGuiManager()->m_guiTeam )
													GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pSelectSkill;
												GetScene()->GetMainCharacter()->UseSkillOnCharacter( pPet->GetNpc() );
											}
											else
											{
												if ( GetGameGuiManager()->m_guiFloatAcessKey )
													GetGameGuiManager()->m_guiFloatAcessKey->OnAccessBtnSelectSkill( id, dwKeyID );
											}                                                                                      											
										}
										else
										{
											GetScene()->GetMainCharacter()->OnSelectSkill( dwKeyID );
											RestoreMouseIcon();
										}
									}								
								}
							}
						}									
						break;
					case GameAccelKeyType_Item:
						{
							CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID( dwKeyID);
							if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
								pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( dwKeyID, OBJECTCELLTYPE_COMMON );
							else
							{
								if ( pItem->GetLocation().dwLocation != OBJECTCELLTYPE_COMMON )
									pItem = NULL;
							}
							/*if ( NULL == pItem )
								pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( dwKeyID, OBJECTCELLTYPE_PACKAGE );*/

							if ( pItem )
							{
								CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
								CALL_USEITEM_P(pMainRole,pItem->GetThisID());								
							}
						}	
						break;
					case GameAccelKeyType_Face:
						{
							stResourceLocation rl;
							int fNum = 0;
							if ( GetGameGuiManager()->m_guiFloatAcessKey->m_pBtnAccessKey[id]->GetBitmapInfo( rl, fNum ) )
							{	
								//GetGameCursor()->SetImage( &rl, stPointI(15,15) );
								GetEmotionInfo()->OnPreUseEmotion(dwKeyID);
								GetEmotionInfo()->SetCurIconInfo(dwKeyID,FROM_FLOAT);
								//GetGameAccelKeyManager()->m_curEditAccessKey.bMainAccessKey = false;
								GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe = PLACE_ACCESSKEY_FLOAT;
								GetGameAccelKeyManager()->m_curEditAccessKey.group = FLOAT_ACCESS_KEY_GROUP;
								GetGameAccelKeyManager()->m_curEditAccessKey.keyNo = id;
								GetGameAccelKeyManager()->m_curEditAccessKey.rl = rl;
								GetGameAccelKeyManager()->m_curEditAccessKey.keyType = keyType;
								GetGameAccelKeyManager()->m_curEditAccessKey.keyID = dwKeyID;

								CGuiButton* btn = GetGameGuiManager()->m_guiFloatAcessKey->m_pBtnAccessKey[id];
								GetGameAccelKeyManager()->m_curEditAccessKey.m_strText = btn->GetText();
								GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont = btn->GetFont();

							}						
						}
						break;
					case GameAccelKeyType_Equipment:
						{
							CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByID( dwKeyID );
							if ( pItem )
								EquipItem( pItem,-1 ); // 一键换装需要修改
						}
						break;
					case GameAccelKeyType_TaoZhuang:
						{
							GetGameGuiManager()->m_guiUserInfo->equipTaoZhuang(dwKeyID);
						}
						break;
					case GameAccelKeyType_AttPak:
						{
							GetGameGuiManager()->m_guiMain->changeAttPak(dwKeyID);
						}
						break;
					}
				}
			}			
		}
	}

	FUNCTION_END;
}

/**
 * \brief 初始化脚本
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("ParseFloatAccessBtnClickEvent",&ParseFloatAccessBtnClickEvent)
	];

	FUNCTION_END;
}

/**
 * \brief 绘制快捷键编号
 * 
 * 
 * 
 * \param fElapsedTime : 
 * \return 
 */
HRESULT CGuiFloatAccessKey::RenderAccessKeyNotes( float fElapsedTime )
{
	FUNCTION_BEGIN;

	//Get access key btn
	int nLeft = 23;
	int nTop = 21;	
	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		if ( m_pBtnAccessKey[i] )
		{
			//Enable btn
			m_pBtnAccessKey[i]->SetEnabled( true );

			//Get position
			stPointI pt;
	           	if ( i != 0 )
				 pt.x =  g_nFloatAccessBtnLeft+(i-1)*g_FloatAccessBtnSpace;
			else
				 pt.x =  g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace;
			pt.y = g_nFloatAccessBtnTop ;


		//	pt = m_pBtnAccessKey[i]->GetLocation();
			pt.x += m_x;
			pt.y += m_y;

			pt.x += nLeft;
			pt.y += nTop;

			char keyName[4];
			sprintf( keyName, "%d", i );

			GetDevice()->DrawString( keyName, pt );

			//Render item num
			enumGameAccelKeyType type;
			DWORD id;
			BYTE key[4];
			key[0] = '0' + i;
			key[1] = 0;
			if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, FLOAT_ACCESS_KEY_GROUP, type, id ) )
			{
				if ( (type == GameAccelKeyType_Item) || (type == GameAccelKeyType_Equipment) )
				{
					CRoleItem* pItem = NULL;
					if ( type == GameAccelKeyType_Item )
					{
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );
						if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
							pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( id, OBJECTCELLTYPE_COMMON );
					}
					else if ( type == GameAccelKeyType_Equipment )
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );

					if ( pItem
						&& ( (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_COMMON)
							|| (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP)
							|| (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_SELL)
							/*|| (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_PACKAGE)*/
							|| (pItem->GetLocation().dwLocation == OBJECTCELLTYPE_STORE) ) )
					{
						//Get offset cause of different screen size
						stPointI pt;
						pt = m_pBtnAccessKey[i]->GetLocation();
						pt.x += m_x + 1;
						pt.y += m_y;

						if ( GetScene()->GetMainCharacter() && (type == GameAccelKeyType_Item) && !pItem->IsAccessNeedThisID() )
						{
							int itemCount = GetScene()->GetMainCharacter()->GetItemCount( pItem->GetObjectID(), OBJECTCELLTYPE_COMMON )
								/*+ GetScene()->GetMainCharacter()->GetItemCount( pItem->GetObjectID(), OBJECTCELLTYPE_PACKAGE )*/;
							if ( itemCount > 0 )
							{
								if ( 0 )
								{
									char itemNum[5];
									sprintf( itemNum, "%d", itemCount );	//
									//GetDevice()->SetFontScale( 0.5f, 0.5f );
									GetDevice()->DrawString( itemNum, pt );
									//GetDevice()->SetFontScale( 1.0f, 1.0f );
								}
								else		//This time we use num image
								{
									//Get num list
									int curNum = itemCount;
									std::list<int> numList;
									numList.clear();

									while ( curNum >= 10 )
									{
										numList.push_front( curNum-(curNum/10)*10 );
										curNum = curNum/10;
									}

									if ( curNum > 0 )
										numList.push_front( curNum );

									//Render num img list
									stRectI	rcSkill;
									rcSkill.left = m_pBtnAccessKey[i]->GetLocation().x + m_x;
									rcSkill.top = m_pBtnAccessKey[i]->GetLocation().y + m_y;
									rcSkill.right = rcSkill.left + 35;
									rcSkill.bottom = rcSkill.top + 35;

									std::list<int>::iterator it;
									for( it=numList.begin();it!=numList.end();it++ )
									{
										if ( (*it >= 0) && (*it < 10) )
										{
											if ( GetGameGuiManager()->m_guiMain && GetGameGuiManager()->m_guiMain->m_itemNumImg[*it] )
											{
												GetGameGuiManager()->m_guiMain->m_itemNumImg[*it]->Render( rcSkill.left, rcSkill.top );
												rcSkill.left += GetGameGuiManager()->m_guiMain->m_itemNumImg[*it]->GetWidth();
												rcSkill.right += GetGameGuiManager()->m_guiMain->m_itemNumImg[*it]->GetWidth();
											}
										}
									}
								}
							}
							else
							{
								//Delete this access key
								GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
								RefreshKeyGroupState();
							}
						}

						//Disable item icon when current equipment is equiped
						if ( type == GameAccelKeyType_Equipment )
						{
							if ( pItem->GetLocation().dwLocation == OBJECTCELLTYPE_EQUIP )
								m_pBtnAccessKey[i]->SetEnabled( false );
						}
					}
					else
					{
						//Delete this access key
						GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
						RefreshKeyGroupState();
					}
				}
			}
		}
	}
	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 更新快捷键提示
 * 
 * 
 * 
 * \param btnID : 快捷键按钮id
 * \return 
 */
void CGuiFloatAccessKey::UpdateKeyTooltips( const BYTE& btnID )
{
	FUNCTION_BEGIN;

	if ( (btnID < 0) || (btnID>=FLOAT_ACCESS_KEY_NUM ) )
		return;

	/*if ( NULL == GetGameGuiManager()->m_guiSkill )
		return;*/

	DWORD id;
	enumGameAccelKeyType type;
	BYTE key[4];
	key[0] = '0'+btnID;
	key[1] = 0;

	if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, (BYTE)FLOAT_ACCESS_KEY_GROUP, type, id ) )
	{
		switch( type )
		{
		case GameAccelKeyType_Skill:
			{
				if ( NULL == GetGameGuiManager()->m_guiSkill )
				{
					GetGameGuiManager()->AddSkill();
					if ( GetGameGuiManager()->m_guiSkill )
					{
						GetGameGuiManager()->m_guiSkill->SetVisible( false );
						m_activeSkillToolTips = GetGameGuiManager()->m_guiSkill->GetSkillIconTooltipsFullFromID( id );
						//GetGameGuiManager()->m_guiSkill->OnClose();
					}
				}
				else
					m_activeSkillToolTips = GetGameGuiManager()->m_guiSkill->GetSkillIconTooltipsFullFromID( id );
			}
			break;
		case GameAccelKeyType_Item:
			{
				//
			}
			break;
		case GameAccelKeyType_Face:
			{
				//m_activeSkillToolTips = GetEmotionInfo()->GetToolTips(id);
			}
		default:
			break;
		}
	}
	else
		m_activeSkillToolTips = NULL;

	FUNCTION_END;
}

void CGuiFloatAccessKey::OnAccessBtnSelectSkill( const int& btnID, const int& skillId )
{
	SkillBase_t* pSelectSkill = GetSkillBase( skillId );
	/*if ( pSelectSkill && ( (pSelectSkill->dwType == SkillType_State) || (pSelectSkill->dwType == SkillType_StateAttack) )
		&& ( (pSelectSkill->dwTarget & SkillTarget_Self) || (pSelectSkill->dwTarget & SkillTarget_Single) ) )*/
	if ( IsCanSelectInHandSkill( pSelectSkill ) )
	{
		stResourceLocation rl;
		if ( m_pBtnAccessKey[btnID] )
		{
			int fNum = 0;
			//m_pBtnAccessKey[btnID]->GetBitmapInfo( rl, fNum );
			stPointI ptCursor(0,0);
			ptCursor.x += m_pBtnAccessKey[btnID]->GetWidth()/2;
			ptCursor.y += m_pBtnAccessKey[btnID]->GetHeight()/2;
			//GetScene()->GetMainCharacter()->m_pActiveSkill = pSelectSkill;
			if ( GetGameGuiManager()->m_guiTeam )
				GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pSelectSkill;

			GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorUseAccess );
			//Engine_GetCursor()->SetImage( &rl, ptCursor );
		}							
	}
}

void CGuiFloatAccessKey::EditAccessKeyBtn( const DWORD& id )
{
	if ( GetGameAccelKeyManager() && GetGameAccelKeyManager()->m_curEditAccessKey.IsValid() )
	{
		enumGameAccelKeyType keyType;
		DWORD dwKeyID;
		BYTE curGroup = FLOAT_ACCESS_KEY_GROUP;
		BYTE key[4];
		key[0] = '0' + id;
		key[1] = 0;

		
		if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )		//cur btn has an access key
		{
			if (GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_FLOAT)
			{
				//Exchange access key
				//Change btn bitmap
				if ( m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]
					&& m_pBtnAccessKey[id] )
				{
				    //sky 字体清除防叠加
				    m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetText("");
					
					int __keyno = GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
					if ( __keyno != 0 )
						m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(__keyno-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
					else
						m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );


					int frmNum;
					//set old
					stResourceLocation rlOld;
					stResourceLocation rl;
					m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->GetBitmapInfo( rlOld, frmNum );
					m_pBtnAccessKey[id]->GetBitmapInfo( rl, frmNum );
					if ( keyType == GameAccelKeyType_Skill || keyType == GameAccelKeyType_TaoZhuang || keyType == GameAccelKeyType_AttPak)
					{
						m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 3, dwState );
					}
					else if( keyType == GameAccelKeyType_Face)
					{
						__SetEmotionAccessBtn(rl,__keyno);
					}
					else
					{
						m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( itemIconScale );
						m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 1 );
					}
					CGuiButton* btn_old = m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
					CGuiButton* btn_new = m_pBtnAccessKey[id];
					btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_old->SetText(btn_new->GetText());
					btn_old->SetFont(btn_new->GetFont());

					btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
					btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);


					if ( id != 0 )
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(id-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
					else
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );


					//set new
					if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill
						|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang
						|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak)
					{
						m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[id]->SetBitmap( rlOld, 3, dwState );
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
					{
						__SetEmotionAccessBtn(rlOld,id);
					}
					else
					{
						m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
						m_pBtnAccessKey[id]->SetBitmap( rlOld, 1 );
					}								
				}
				//delete old two
				{
					//delete old edit
					{
						BYTE key[4];
						key[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						key[1] = 0;
						GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
					}

					//delete old cur
					{
						BYTE key[4];
						key[0] = '0' + id;
						key[1] = 0;
						GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
					}
				}
				//Add new two
				{
					//add new edit
					{
						stGameAccelKey key;
						key.type = keyType;
						key.dwID = dwKeyID;
						key.szAccelKey[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						key.szAccelKey[1] = 0;
						key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

						GetGameAccelKeyManager()->SetKey(key);
						GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
					}
					//add new cur
					{
						stGameAccelKey key;
						key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
						key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
						key.szAccelKey[0] = '0' + id;
						key.szAccelKey[1] = 0;
						key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

						GetGameAccelKeyManager()->SetKey(key);
						GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
					}
				}

				RefreshKeyGroupState();

				RestoreMouseIcon();
			}
			else if(GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_MAIN)
			{
				if ( GetGameGuiManager()->m_guiMain
					&& GetGameGuiManager()->m_guiMain->IsVisible() )
				{
					CGuiMain* pMaindlg = GetGameGuiManager()->m_guiMain;

					//exchange with float cur access key
					//Change btn bitmap
					if ( pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]
						&& m_pBtnAccessKey[id] )
					{
						int __keyno = GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						if ( __keyno != 0 )
							m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(__keyno-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
						else
							m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );

						int frmNum;
						//set old
						stResourceLocation rlOld;
						stResourceLocation rl;
						pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->GetBitmapInfo( rlOld, frmNum );
						m_pBtnAccessKey[id]->GetBitmapInfo( rl, frmNum );
						if ( keyType == GameAccelKeyType_Skill || keyType == GameAccelKeyType_TaoZhuang || keyType == GameAccelKeyType_AttPak )
						{
							pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( skillIconScale );
							pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 3, dwState );
						}
						else if( keyType == GameAccelKeyType_Face)
						{
							pMaindlg->__SetEmotionAccessBtn(rl,GetGameAccelKeyManager()->m_curEditAccessKey.keyNo);
						}		
						else
						{
							pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( itemIconScale );
							pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 1 );
						}
						CGuiButton* btn_old = pMaindlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
						CGuiButton* btn_new = m_pBtnAccessKey[id];
						btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
						btn_old->SetText(btn_new->GetText());
						btn_old->SetFont(btn_new->GetFont());

						btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
						btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
						btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);
						
						//set new
						if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill 
							|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang
							|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak )
						{
							m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
							m_pBtnAccessKey[id]->SetBitmap( rlOld, 3, dwState );
						}
						else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
						{
							__SetEmotionAccessBtn(rlOld,id);
						}
						else
						{
							m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
							m_pBtnAccessKey[id]->SetBitmap( rlOld, 1 );
						}
					}
					//delete old two
					{
						//delete old edit
						{
							BYTE key[4];
							key[0] = VK_F1+GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key[1] = 0;
							GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}

						//delete old cur
						{
							BYTE key[4];
							key[0] = '0' + id;
							key[1] = 0;
							GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
						}
					}
					//Add new two
					{
						//add new edit
						{
							stGameAccelKey key;
							key.type = keyType;
							key.dwID = dwKeyID;
							key.szAccelKey[0] = VK_F1 + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key.szAccelKey[1] = 0;
							key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

							GetGameAccelKeyManager()->SetKey(key);
							GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}
						//add new cur
						{
							stGameAccelKey key;
							key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
							key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
							key.szAccelKey[0] = '0' + id;
							key.szAccelKey[1] = 0;
							key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

							GetGameAccelKeyManager()->SetKey(key);
							GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
						}
					}

					RefreshKeyGroupState();
					pMaindlg->RefreshKeyGroupState( pMaindlg->GetCurAccessKeyGroup() );

					RestoreMouseIcon();
				}
			}
			else if(GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_OTHER)
			{
				if ( GetGameGuiManager()->m_guiOtherAccessKey
					&& GetGameGuiManager()->m_guiOtherAccessKey->IsVisible() )
				{
					CGuiOtherAccessKey* pOtherAccessKeydlg = GetGameGuiManager()->m_guiOtherAccessKey;
					//exchange with float cur access key
					//Change btn bitmap
					if ( pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]
					&& m_pBtnAccessKey[id] )
					{
						int __keyno = GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						if ( __keyno != 0 )
							m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(__keyno-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
						else
							m_pBtnAccessKey[__keyno]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );

						int frmNum;
						//set old
						stResourceLocation rlOld;
						stResourceLocation rl;
						pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->GetBitmapInfo( rlOld, frmNum );
						m_pBtnAccessKey[id]->GetBitmapInfo( rl, frmNum );
						if ( keyType == GameAccelKeyType_Skill ||  keyType == GameAccelKeyType_TaoZhuang ||  keyType == GameAccelKeyType_AttPak)
						{
							pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( skillIconScale );
							pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 3, dwState );
						}
						else if( keyType == GameAccelKeyType_Face)
						{
							pOtherAccessKeydlg->__SetEmotionAccessBtn(rl,GetGameAccelKeyManager()->m_curEditAccessKey.keyNo);
						}
						else
						{
							pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( itemIconScale );
							pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 1 );
						}
						//sky 字体处理
						CGuiButton* btn_old = pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
						CGuiButton* btn_new = m_pBtnAccessKey[id];
						btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
						btn_old->SetText(btn_new->GetText());
						btn_old->SetFont(btn_new->GetFont());
						btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
						btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
						btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);

						//set new
						if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill
							|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang
							|| GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak)
						{
							m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
							m_pBtnAccessKey[id]->SetBitmap( rlOld, 3, dwState );
						}
						else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
						{
							__SetEmotionAccessBtn(rlOld,id);
						}
						else
						{
							m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
							m_pBtnAccessKey[id]->SetBitmap( rlOld, 1 );
						}							
					}

					//delete old two
					{
						//delete old edit
						{
							BYTE key[4];
							key[0] = VK_CONTROL;
							key[1] = VK_F1+GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key[2] = 0;
							GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}

						//delete old cur
						{
							BYTE key[4];
							key[0] = '0' + id;
							key[1] = 0;
							GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
						}
					}
					//Add new two
					{
						//add new edit
						{
							stGameAccelKey key;
							key.type = keyType;
							key.dwID = dwKeyID;
							key.szAccelKey[0] = VK_CONTROL;
							key.szAccelKey[1] = VK_F1+GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key.szAccelKey[2] = 0;
							key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

							GetGameAccelKeyManager()->SetKey(key);
							GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}
						//add new cur
						{
							stGameAccelKey key;
							key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
							key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
							key.szAccelKey[0] = '0' + id;
							key.szAccelKey[1] = 0;
							key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

							GetGameAccelKeyManager()->SetKey(key);
							GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
						}
					}

					pOtherAccessKeydlg->RefreshKeyGroupState();
					RefreshKeyGroupState();

					/* if ( GetGameGuiManager()->m_guiMain
						&& GetGameGuiManager()->m_guiMain->IsVisible() )
					{
						{
							BYTE key[4];
							key[0] = VK_F1 + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key[1] = 0;
							GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}
						{
							stGameAccelKey key;
							key.type = keyType;
							key.dwID = dwKeyID;
							key.szAccelKey[0] = VK_F1 + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
							key.szAccelKey[1] = 0;
							key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

							GetGameAccelKeyManager()->SetKey(key);
							GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
						}

						CGuiMain* pMaindlg = GetGameGuiManager()->m_guiMain;
						pMaindlg->RefreshKeyGroupState(pMaindlg->GetCurAccessKeyGroup() );
					} */

					RestoreMouseIcon();
				}
			}

		}
		else		//cur btn has no access key
		{
			if (GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_FLOAT)
			{
				//delete old edit
				{
					BYTE key[4];
					key[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
					key[1] = 0;
					GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
				}
				RemoveAccessKeyBtnInfo( (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.keyNo );

				if ( id != 0 )
					m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(id-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
				else
					m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop);


				//save cur access to this pos
				if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
				{
					m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
					m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
				}
				else if ( (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Item)
						|| (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Equipment) )
				{
					m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
					m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 1 );
				}	
				else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
				{
					__SetEmotionAccessBtn(GetGameAccelKeyManager()->m_curEditAccessKey.rl,id);
				}				
				else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang)
				{
					m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
				}
				else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak)
				{
					m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
				}
				CGuiButton* btn_old = m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
				CGuiButton* btn_new = m_pBtnAccessKey[id];
				btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
				btn_old->SetText(btn_new->GetText());
				btn_old->SetFont(btn_new->GetFont());

				btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
				btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
				btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);

				//Save access key change
				stGameAccelKey key;
				key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
				key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
				key.szAccelKey[0] = '0' + id;
				key.szAccelKey[1] = 0;
				key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

				GetGameAccelKeyManager()->SetKey(key);
				GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

				RestoreMouseIcon();
			}
			else if(GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_MAIN)
			{
				if ( GetGameGuiManager()->m_guiMain
					&& GetGameGuiManager()->m_guiMain->IsVisible() )
				{
					CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;
					//delete old edit
					{
						BYTE key[4];
						key[0] = VK_F1 + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						key[1] = 0;
						GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
					}
					pMainDlg->RemoveAccessKeyBtnInfo( (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.keyNo );

					if ( id != 0 )
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(id-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
					else
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );

					//put to float access key
					if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
					{
						m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang)
					{
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
						m_pBtnAccessKey[id]->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
						m_pBtnAccessKey[id]->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak)
					{
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );

					}
					else if ( (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Item)
							|| (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Equipment) )
					{
						m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 1 );
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
					{
						__SetEmotionAccessBtn(GetGameAccelKeyManager()->m_curEditAccessKey.rl,id);
					}
					CGuiButton* btn_old = pMainDlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
					CGuiButton* btn_new = m_pBtnAccessKey[id];
				    btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_old->SetText(btn_new->GetText());
					btn_old->SetFont(btn_new->GetFont());

					btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
					btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);

					//Save access key change
					stGameAccelKey key;
					key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
					key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
					key.szAccelKey[0] = '0' + id;
					key.szAccelKey[1] = 0;
					key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

					GetGameAccelKeyManager()->SetKey(key);
					GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
				}

				RestoreMouseIcon();
			}
			else if(GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe == PLACE_ACCESSKEY_OTHER)
			{
				Engine_WriteLog("放下物品:PLACE_ACCESSKEY_OTHER\n");
				if ( GetGameGuiManager()->m_guiOtherAccessKey
					&& GetGameGuiManager()->m_guiOtherAccessKey->IsVisible() )
				{
					CGuiOtherAccessKey* pOtherAccessKeydlg = GetGameGuiManager()->m_guiOtherAccessKey;
					//delete old edit
					{
						BYTE key[4];
						key[0] = VK_CONTROL;
						key[1] = VK_F1 + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
						key[2] = 0;
						GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
					}
					pOtherAccessKeydlg->RemoveAccessKeyBtnInfo( (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.keyNo );

					if ( id != 0 )
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(id-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
					else
						m_pBtnAccessKey[id]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );

					//put to main access key
					if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
					{
						m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_TaoZhuang)
					{
						m_pBtnAccessKey[id]->SetBitmapScale( skillIconScale ); //没有的
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );
					
						CGuiButton* btn = m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
						m_pBtnAccessKey[id]->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
						m_pBtnAccessKey[id]->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);
					}
					else if( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_AttPak)
					{
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3, dwState );

					}
					else if ( (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Item)
						|| (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Equipment) )
					{
						m_pBtnAccessKey[id]->SetBitmapScale( itemIconScale );
						m_pBtnAccessKey[id]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 1 );
					}
					
					else if  (GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Face)
					{
						__SetEmotionAccessBtn(GetGameAccelKeyManager()->m_curEditAccessKey.rl,id);
					}
					CGuiButton* btn_old = pOtherAccessKeydlg->m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo];
					CGuiButton* btn_new = m_pBtnAccessKey[id];
					btn_old->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_old->SetText(btn_new->GetText());
					btn_old->SetFont(btn_new->GetFont());
					btn_new->SetTextColor(COLOR_ARGB(255,255,0,0));
					btn_new->SetText(GetGameAccelKeyManager()->m_curEditAccessKey.m_strText.c_str());
					btn_new->SetFont(GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont);

					//Save access key change
					stGameAccelKey key;
					key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
					key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
					key.szAccelKey[0] = '0' + id;
					key.szAccelKey[1] = 0;
					key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

					GetGameAccelKeyManager()->SetKey(key);
					GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
				}

				RestoreMouseIcon();
			}
			
		}
	}
}

bool CGuiFloatAccessKey::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;
	
	switch( nEvent )
	{
	case EVENT_BUTTON_DRAG_MOVE:
		if ( (nID >= 100) && (nID < 100+FLOAT_ACCESS_KEY_NUM) )
		{
			nID -= 100;
			if ( !GetGameCursor()->IsDrag()
				&& !(GetGameCursor()->GetCursorImageType() == eCursorSetAccess)
				&& !(GetGameCursor()->GetCursorImageType() == eCursorUseAccess)
				&& !(GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
			{
				enumGameAccelKeyType keyType;
				DWORD dwKeyID;
				BYTE curGroup = FLOAT_ACCESS_KEY_GROUP;
				BYTE key[4];
				key[0] = '0' + nID;
				key[1] = 0;
				if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )
				{
					if ( m_pBtnAccessKey[nID] )
					{
						if ( (abs(m_pBtnAccessKey[nID]->GetDragOffset().x) >= 5)
							|| (abs(m_pBtnAccessKey[nID]->GetDragOffset().y) >= 5) )
						{
							stResourceLocation rl;
							int fNum = 0;
							if ( m_pBtnAccessKey[nID]->GetBitmapInfo( rl, fNum ) )
							{
								GetGameCursor()->SetCursor( CCursor::CursorType_Drag, eCursorEditAccess );
								GetGameCursor()->SetImage( &rl, stPointI(15,15) );
								if ( GetGameAccelKeyManager() )
								{
									//GetGameAccelKeyManager()->m_curEditAccessKey.bMainAccessKey = false;
									GetGameAccelKeyManager()->m_curEditAccessKey.placeAccessKe = PLACE_ACCESSKEY_FLOAT;
									GetGameAccelKeyManager()->m_curEditAccessKey.group = FLOAT_ACCESS_KEY_GROUP;
									GetGameAccelKeyManager()->m_curEditAccessKey.keyNo = nID;
									GetGameAccelKeyManager()->m_curEditAccessKey.rl = rl;
									GetGameAccelKeyManager()->m_curEditAccessKey.keyType = keyType;
									GetGameAccelKeyManager()->m_curEditAccessKey.keyID = dwKeyID;

									CGuiButton* btn = m_pBtnAccessKey[nID];
									GetGameAccelKeyManager()->m_curEditAccessKey.m_strText = btn->GetText();
									GetGameAccelKeyManager()->m_curEditAccessKey.m_iFont = btn->GetFont();


									if( GameAccelKeyType_Face == keyType )
									{
										GetEmotionInfo()->SetCurIconInfo(dwKeyID,FROM_FLOAT);
									}
								}
							}
						}
					}			
				}					
			}
		}
		break;
	case EVENT_BUTTON_DRAG_END:
		//if ( (nID >=100) && (nID < 100+FLOAT_ACCESS_KEY_NUM) )
		//{
		//	nID -= 100;
		//	if ( GetGameCursor()->IsDrag()
		//		&& (GetGameCursor()->GetCursorImageType() == eCursorEditAccess) )
		//	{
		//		if ( m_mouseOverAccessKeyNo != -1 )		//Cur pos has access key btn
		//		{
		//			if ( GetGameAccelKeyManager() && GetGameAccelKeyManager()->m_curEditAccessKey.IsValid() )
		//			{
		//				enumGameAccelKeyType keyType;
		//				DWORD dwKeyID;
		//				BYTE curGroup = FLOAT_ACCESS_KEY_GROUP;
		//				BYTE key[4];
		//				key[0] = '0' + m_mouseOverAccessKeyNo;
		//				key[1] = 0;
		//				if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )		//cur btn has an access key
		//				{
		//					//Exchange access key
		//					//Change btn bitmap
		//					if ( m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]
		//					&& m_pBtnAccessKey[m_mouseOverAccessKeyNo] )
		//					{
		//						int frmNum;
		//						//set old
		//						stResourceLocation rlOld;
		//						stResourceLocation rl;
		//						m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->GetBitmapInfo( rlOld, frmNum );
		//						m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetBitmapInfo( rl, frmNum );
		//						if ( keyType == GameAccelKeyType_Skill )
		//						{
		//							m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( skillIconScale );
		//							m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 3, dwState );
		//						}
		//						else
		//						{
		//							m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( itemIconScale );
		//							m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 1 );
		//						}
		//						
		//						//set new
		//						if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
		//						{
		//							m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmapScale( skillIconScale );
		//							m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmap( rlOld, 3 );
		//						}
		//						else
		//						{
		//							m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmapScale( itemIconScale );
		//							m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmap( rlOld, 1 );
		//						}								
		//					}
		//					//delete old two
		//					{
		//						//delete old edit
		//						{
		//							BYTE key[4];
		//							key[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//							key[1] = 0;
		//							GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//						}

		//						//delete old cur
		//						{
		//							BYTE key[4];
		//							key[0] = '0' + m_mouseOverAccessKeyNo;
		//							key[1] = 0;
		//							GetGameAccelKeyManager()->DeleteOneAccelKey( key, FLOAT_ACCESS_KEY_GROUP );
		//						}
		//					}
		//					//Add new two
		//					{
		//						//add new edit
		//						{
		//							stGameAccelKey key;
		//							key.type = keyType;
		//							key.dwID = dwKeyID;
		//							key.szAccelKey[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//							key.szAccelKey[1] = 0;
		//							key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

		//							GetGameAccelKeyManager()->SetKey(key);
		//							GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//						}
		//						//add new cur
		//						{
		//							stGameAccelKey key;
		//							key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
		//							key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
		//							key.szAccelKey[0] = '0' + m_mouseOverAccessKeyNo;
		//							key.szAccelKey[1] = 0;
		//							key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

		//							GetGameAccelKeyManager()->SetKey(key);
		//							GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );
		//						}
		//					}

		//					RefreshKeyGroupState();

		//					RestoreMouseIcon();
		//				}
		//				else		//cur btn has no access key
		//				{
		//					//delete old edit
		//					{
		//						BYTE key[4];
		//						key[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//						key[1] = 0;
		//						GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//					}
		//					RemoveAccessKeyBtnInfo( (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.keyNo );

		//					//save cur access to this pos
		//					if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
		//					{
		//						m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmapScale( skillIconScale );
		//						m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3 );
		//					}
		//					else if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Item )
		//					{
		//						m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmapScale( itemIconScale );
		//						m_pBtnAccessKey[m_mouseOverAccessKeyNo]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 1 );
		//					}							

		//					//Save access key change
		//					stGameAccelKey key;
		//					key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
		//					key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
		//					key.szAccelKey[0] = '0' + m_mouseOverAccessKeyNo;
		//					key.szAccelKey[1] = 0;
		//					key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

		//					GetGameAccelKeyManager()->SetKey(key);
		//					GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

		//					RestoreMouseIcon();
		//				}
		//			}

		//		}
		//		else
		//		{
		//			if ( GetGameGuiManager()->m_guiMain
		//				&& GetGameGuiManager()->m_guiMain->IsVisible()
		//				&& (GetGameGuiManager()->m_guiMain->GetMouseOverAccessKeyNo() != -1 ) )
		//			{
		//				CGuiMain* pMaindlg = GetGameGuiManager()->m_guiMain;

		//				if ( GetGameAccelKeyManager() && GetGameAccelKeyManager()->m_curEditAccessKey.IsValid() )
		//				{
		//					int mouseOverAccessKeyNo = pMaindlg->GetCurAccessKeyNo();

		//					enumGameAccelKeyType keyType;
		//					DWORD dwKeyID;
		//					BYTE curGroup = pMaindlg->GetCurAccessKeyGroup();
		//					BYTE key[4];
		//					key[0] = VK_F1 + mouseOverAccessKeyNo;
		//					key[1] = 0;
		//					if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )		//cur btn has an access key
		//					{
		//						//exchange with float cur access key
		//						//Change btn bitmap
		//						if ( m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]
		//						&& pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo] )
		//						{
		//							int frmNum;
		//							//set old
		//							stResourceLocation rlOld;
		//							stResourceLocation rl;
		//							m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->GetBitmapInfo( rlOld, frmNum );
		//							pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->GetBitmapInfo( rl, frmNum );
		//							if ( keyType == GameAccelKeyType_Skill )
		//							{
		//								m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( skillIconScale );
		//								m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 3, dwState );
		//							}
		//							else
		//							{
		//								m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmapScale( itemIconScale );
		//								m_pBtnAccessKey[GetGameAccelKeyManager()->m_curEditAccessKey.keyNo]->SetBitmap( rl, 1 );
		//							}									

		//							//set new
		//							if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
		//							{
		//								pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmapScale( skillIconScale );
		//								pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmap( rlOld, 3 );
		//							}
		//							else
		//							{
		//								pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmapScale( itemIconScale );
		//								pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmap( rlOld, 1 );
		//							}									
		//						}
		//						//delete old two
		//						{
		//							//delete old edit
		//							{
		//								BYTE key[4];
		//								key[0] = '0'+GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//								key[1] = 0;
		//								GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//							}

		//							//delete old cur
		//							{
		//								BYTE key[4];
		//								key[0] = VK_F1 + mouseOverAccessKeyNo;
		//								key[1] = 0;
		//								GetGameAccelKeyManager()->DeleteOneAccelKey( key, pMaindlg->GetCurAccessKeyGroup() );
		//							}
		//						}
		//						//Add new two
		//						{
		//							//add new edit
		//							{
		//								stGameAccelKey key;
		//								key.type = keyType;
		//								key.dwID = dwKeyID;
		//								key.szAccelKey[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//								key.szAccelKey[1] = 0;
		//								key.mGroup = (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.group;

		//								GetGameAccelKeyManager()->SetKey(key);
		//								GetGameAccelKeyManager()->Save( GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//							}
		//							//add new cur
		//							{
		//								stGameAccelKey key;
		//								key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
		//								key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
		//								key.szAccelKey[0] = VK_F1 + mouseOverAccessKeyNo;
		//								key.szAccelKey[1] = 0;
		//								key.mGroup = pMaindlg->GetCurAccessKeyGroup();

		//								GetGameAccelKeyManager()->SetKey(key);
		//								GetGameAccelKeyManager()->Save( pMaindlg->GetCurAccessKeyGroup() );
		//							}
		//						}

		//						RefreshKeyGroupState();
		//						pMaindlg->RefreshKeyGroupState( pMaindlg->GetCurAccessKeyGroup() );

		//						RestoreMouseIcon();
		//					}
		//					else
		//					{
		//						//delete old edit
		//						{
		//							BYTE key[4];
		//							key[0] = '0' + GetGameAccelKeyManager()->m_curEditAccessKey.keyNo;
		//							key[1] = 0;
		//							GetGameAccelKeyManager()->DeleteOneAccelKey( key, GetGameAccelKeyManager()->m_curEditAccessKey.group );
		//						}
		//						RemoveAccessKeyBtnInfo( (BYTE)GetGameAccelKeyManager()->m_curEditAccessKey.keyNo );

		//						//put to main access key
		//						if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Skill )
		//						{
		//							pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmapScale( skillIconScale );
		//							pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 3 );
		//						}
		//						else if ( GetGameAccelKeyManager()->m_curEditAccessKey.keyType == GameAccelKeyType_Item )
		//						{
		//							pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmapScale( itemIconScale );
		//							pMaindlg->m_pBtnAccessKey[mouseOverAccessKeyNo]->SetBitmap( GetGameAccelKeyManager()->m_curEditAccessKey.rl, 1 );
		//						}								

		//						//Save access key change
		//						stGameAccelKey key;
		//						key.type = GetGameAccelKeyManager()->m_curEditAccessKey.keyType;
		//						key.dwID = GetGameAccelKeyManager()->m_curEditAccessKey.keyID;
		//						key.szAccelKey[0] = VK_F1 + mouseOverAccessKeyNo;
		//						key.szAccelKey[1] = 0;
		//						key.mGroup = (BYTE)pMaindlg->GetCurAccessKeyGroup();

		//						GetGameAccelKeyManager()->SetKey(key);
		//						GetGameAccelKeyManager()->Save( pMaindlg->GetCurAccessKeyGroup() );

		//						RestoreMouseIcon();
		//					}
		//				}						
		//			}
		//			else		//delete this access key
		//			{
		//				if ( GetGameAccelKeyManager() && !(GetGameAccelKeyManager()->m_curEditAccessKey.bMainAccessKey) )
		//				{
		//					if ( nID == (GetGameAccelKeyManager()->m_curEditAccessKey.keyNo) )
		//					{
		//						//Really has a key to delete
		//						BYTE key[4];
		//						key[0] = '0' + nID;
		//						key[1] = 0;

		//						if ( GetGameAccelKeyManager()->DeleteOneAccelKey( key, (BYTE)FLOAT_ACCESS_KEY_GROUP ) )
		//						{
		//							RemoveAccessKeyBtnInfo( (BYTE)nID );
		//							RestoreMouseIcon();
		//						}
		//					}
		//				}	
		//			}

		//		}
		//	}
		//}
		break;
	case EVENT_LBUTTON_DOWN:
		//if ( (nID >= 100) && (nID <= 109) )
		//{
		//	if ( !GetGameCursor()->IsDrag()
		//		&& !(GetGameCursor()->GetCursorImageType() == eCursorSetAccess) )
		//	{
		//		//if this is a state skill
		//		enumGameAccelKeyType keyType;
		//		DWORD dwKeyID;
		//		BYTE curGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;
		//		BYTE key[4];
		//		key[0] = '0' + nID - 100;
		//		key[1] = 0;
		//		if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )
		//		{
		//			if ( GameAccelKeyType_Skill == keyType )
		//				OnAccessBtnSelectSkill( nID-100, dwKeyID );
		//		}			
		//		//end
		//	}
		//	else if ( GetGameCursor()->IsDrag()
		//			&& (GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
		//	{
		//		OnFindAccessSkill();
		//	}
		//}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 获取鼠标指向的快捷键按钮id
 * 
 * 
 * 
 * \return 鼠标指向的快捷键按钮id
 */
int CGuiFloatAccessKey::GetMouseOverAccessKeyNo()
{
	FUNCTION_BEGIN;

	if ( GetGuiManager()->GetMouseOverDlg() != this )
		return -1;

	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		if ( m_pBtnAccessKey[i]->IsMouseOver() )
		{
			UpdateKeyTooltips( (BYTE)i );
			return i;
			break;
		}
	}

	return -1;

	FUNCTION_END;
}

/**
 * \brief 绘制快捷键提示
 * 
 * 
 * 
 * \param fElapsedTime : 
 * \return 
 */
HRESULT CGuiFloatAccessKey::RenderAccessKeyBtnToolTips( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( (m_mouseOverAccessKeyNo < 0) || (m_mouseOverAccessKeyNo >= FLOAT_ACCESS_KEY_NUM ) )
		return S_OK;

	//Just render mouse on btn tooltips
	//Get mouse btn access key type
	enumGameAccelKeyType type;
	DWORD id;
	BYTE key[4];
	key[0] = '0' + m_mouseOverAccessKeyNo;
	key[1] = 0;
	if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, FLOAT_ACCESS_KEY_GROUP, type, id ) )
	{
		//Get offset cause of different screen size
		stPointI pt;
		pt = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation();
		pt.x += m_x;
		pt.y += m_y;

		pt.x += 0;
		pt.y += 0;		

		switch( type )
		{
		case GameAccelKeyType_Item:
		case GameAccelKeyType_Equipment:
			{
				CRoleItem* pItem = NULL;
				if ( type == GameAccelKeyType_Item )
				{
					pItem = GetScene()->GetMainCharacter()->FindItemByID( id );
					if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
						pItem = GetScene()->GetMainCharacter()->FindItemByLocationConstID( id, OBJECTCELLTYPE_COMMON );
				}
				else if( type == GameAccelKeyType_Equipment )
					pItem = GetScene()->GetMainCharacter()->FindItemByID( id );

				if ( pItem )
				{
					pt.x -= pItem->GetToolTips()->GetWidth();
					pt.y -= pItem->GetToolTips()->GetHeight();

					if ( pt.x < 0 )
						pt.x = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().x + m_x + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetWidth()/2;
					if ( pt.y < 0 )
						pt.y = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().y + m_y + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetHeight();

					pItem->ShowToolTips( pt );
				}
			}
			break;
		case GameAccelKeyType_Skill:
			{
				if ( m_activeSkillToolTips && m_pBtnAccessKey[m_mouseOverAccessKeyNo] )
				{
					pt.x -= m_activeSkillToolTips->GetWidth();
					pt.y -= m_activeSkillToolTips->GetHeight();

					if ( pt.x < 0 )
						pt.x = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().x + m_x + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetWidth()/2;
					if ( pt.y < 0 )
						pt.y = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().y + m_y + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetHeight();
					//Set the same type as item
					m_activeSkillToolTips->SetBkColor(COLOR_ARGB(128,0,0,0));

					if ( NULL == GetGuiManager()->GetToolTips() )
						GetGuiManager()->SetToolTips( m_activeSkillToolTips, pt );

					//restore
					//m_pBtnAccessKeyToolTips[m_mouseOverAccessKeyNo]->SetBkColor(COLOR_ARGB(0,255,255,255));
				}				
			}
			break;
		case GameAccelKeyType_TaoZhuang:
			{
				CToolTips* tips = GetGameGuiManager()->m_guiUserInfo->GetTaoZhuangTooltips( id );

				if ( tips && m_pBtnAccessKey[m_mouseOverAccessKeyNo] )
				{
					pt.x -= tips->GetWidth();
					pt.y -= tips->GetHeight();

					if ( pt.x < 0 )
						pt.x = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().x + m_x + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetWidth()/2;

					//Set the same type as item
					tips->SetBkColor(COLOR_ARGB(128,0,0,0));

					if ( NULL == GetGuiManager()->GetToolTips() )
						GetGuiManager()->SetToolTips( tips, pt );
				}
			}
			break;
		case GameAccelKeyType_AttPak:
			{
				CToolTips* tips = GetGameGuiManager()->m_guiMain->GetAttPakTooltips( id );

				if ( tips && m_pBtnAccessKey[m_mouseOverAccessKeyNo] )
				{
					pt.x -= tips->GetWidth();
					pt.y -= tips->GetHeight();

					if ( pt.x < 0 )
						pt.x = m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetLocation().x + m_x + m_pBtnAccessKey[m_mouseOverAccessKeyNo]->GetWidth()/2;

					//Set the same type as item
					tips->SetBkColor(COLOR_ARGB(128,0,0,0));

					if ( NULL == GetGuiManager()->GetToolTips() )
						GetGuiManager()->SetToolTips( tips, pt );
				}
			}
			break;
		case GameAccelKeyType_Face:
			{
                              CToolTips * pToolTips = GetEmotionInfo()->GetToolTips(id);
			      pt.x -= pToolTips->GetWidth();
			      pt.y -= pToolTips->GetHeight();
			      if ( NULL == GetGuiManager()->GetToolTips() )
				 GetGuiManager()->SetToolTips( pToolTips, pt );

			}
			break;
		default:
			break;
		}
	}

	return S_OK;

	FUNCTION_END;
}

/**
 * \brief 绘制快捷键按钮指向的技能的冷却状态效果
 * 
 * 
 * 
 * \param fElapsedTime : 
 * \return 
 */
HRESULT CGuiFloatAccessKey::RenderAccessKeyBtnDisableStatus( float fElapsedTime )
{
	FUNCTION_BEGIN;

	//Get offset cause of different screen size
	stPointI ptOffset;
	ptOffset.x += m_x + 15;
	ptOffset.y += m_y + 10;

	for	( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		enumGameAccelKeyType type;
		DWORD id;
		BYTE key[4];
		key[0] = '0' + i;
		key[1] = 0;

		if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, FLOAT_ACCESS_KEY_GROUP, type, id ) )
		{
			switch( type )
			{
			case GameAccelKeyType_Item:	
			case GameAccelKeyType_Equipment:
				if ( m_pBtnAccessKey[i] )
				{
					CRoleItem* pItem = NULL;
					if ( type == GameAccelKeyType_Item )
					{
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );
						if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
							pItem = GetScene()->GetMainCharacter()->FindItemByConstID( id );
					}
					else if ( type == GameAccelKeyType_Equipment )
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );

					//Add used sing img
					if ( pItem )
					{
						if ( (pItem->GetObjectID() == c_nXuanYuanFu)
							|| (pItem->GetObjectID() == c_nXianTianFu)
							|| (pItem->GetObjectID() == 666) )
						{
							if ( pItem->GetObject()->maker[0] != 0 )	//Used
							{
								IBitmaps* pBmp=NULL;
								pBmp = GetDevice()->FindBitmaps(&(stResourceLocation("data\\interfaces.gl",3,300)));
								if ( pBmp )
								{
									stRectI	rcSkill;
									rcSkill.left = m_pBtnAccessKey[i]->GetLocation().x + m_x;
									rcSkill.top = m_pBtnAccessKey[i]->GetLocation().y + m_y;
									rcSkill.right = rcSkill.left + 35;
									rcSkill.bottom = rcSkill.top + 35;
									pBmp->RenderAni( rcSkill.left-2-56, rcSkill.top-2-56, xtimeGetTime(), -1, Blend_Null);
								} 
							}
						}
					}
				}
				break;
			case GameAccelKeyType_Skill:
				if ( m_pBtnAccessKey[i] )			//
				{
					SkillBase_t* pSkill = GetSkillBase( id );
					if ( pSkill )
					{
						//TRACE( "Draw skill disable status once\n" );
						stRectI	rcSkill;
						rcSkill.left = m_pBtnAccessKey[i]->GetLocation().x + m_x;
						rcSkill.top = m_pBtnAccessKey[i]->GetLocation().y + m_y;
						rcSkill.right = rcSkill.left + 35;
						rcSkill.bottom = rcSkill.top + 35;
						GetScene()->GetMainCharacter()->DrawSkillDisableStatus( pSkill, &rcSkill );		//( m_pBtnAccessKey[i]->GetBoundingBox() )

						//Draw skill select sign
						if ( GetScene()->GetMainCharacter()->m_pActiveSkill == pSkill )
						{
							//Draw
							if ( GetGameGuiManager()->m_guiMain->m_activeSkillSign )
							{
								GetGameGuiManager()->m_guiMain->m_activeSkillSign->RenderAni( rcSkill.left-4, rcSkill.top-4, xtimeGetTime(), -1 );
							}
						}
					}
				}
				break;
			case GameAccelKeyType_TaoZhuang:
				if ( m_pBtnAccessKey[i] )
				{
					//TRACE( "Draw skill disable status once\n" );
					stRectI	rcSkill;
					rcSkill.left = m_pBtnAccessKey[i]->GetLocation().x + m_x;
					rcSkill.top = m_pBtnAccessKey[i]->GetLocation().y + m_y;
					rcSkill.right = rcSkill.left + 35;
					rcSkill.bottom = rcSkill.top + 35;

					//Draw skill select sign
					if ( GetGameGuiManager()->m_guiUserInfo->getUsedIndex() == id)
					{
						//Draw
						if ( GetGameGuiManager()->m_guiMain->m_activeSkillSign )
						{
							GetGameGuiManager()->m_guiMain->m_activeSkillSign->RenderAni( rcSkill.left-4, rcSkill.top-4, xtimeGetTime(), -1 );
						}
					}
				}
				break;
			case GameAccelKeyType_AttPak: 
				if ( m_pBtnAccessKey[i] )
				{
					//TRACE( "Draw skill disable status once\n" );
					stRectI	rcSkill;
					rcSkill.left = m_pBtnAccessKey[i]->GetLocation().x + m_x;
					rcSkill.top = m_pBtnAccessKey[i]->GetLocation().y + m_y;
					rcSkill.right = rcSkill.left + 35;
					rcSkill.bottom = rcSkill.top + 35;

					//Draw skill select sign
					if (GetScene()->GetMainCharacter()->GetPKMode() + 1 == id)
					{
						//Draw
						if ( GetGameGuiManager()->m_guiMain->m_activeSkillSign )
						{
							GetGameGuiManager()->m_guiMain->m_activeSkillSign->RenderAni( rcSkill.left-4, rcSkill.top-4, xtimeGetTime(), -1 );
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}

	return S_OK;

	FUNCTION_END;
}

void CGuiFloatAccessKey::ResetDlgLocation()
{
	//Set pos follow chat select dlg
	if ( GetGameGuiManager()->m_guiChatSelect && GetGameGuiManager()->m_guiChatSelect->IsVisible() )
	{
		m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 422;
		m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 684 - GetHeight();

		if ( m_y > (GetDevice()->GetHeight() - 97 ) )
		m_y = GetDevice()->GetHeight()-97;
	}
	else
	{
		m_x = 422 + (GetDevice()->GetWidth() - 1024)/2;
		m_y = 684 + (GetDevice()->GetHeight() - 768) - GetHeight();		//		//671
	}

	//Make sure this dlg is nearby float main dlg
	if ( GetGameGuiManager()->m_guiFloatControl )
	{
		stPointI pt;
		pt = GetGameGuiManager()->m_guiFloatControl->GetLocation();
		int width, height, devWidth, devHeight;
		width = GetGameGuiManager()->m_guiFloatControl->GetWidth();
		height = GetGameGuiManager()->m_guiFloatControl->GetHeight();

		devWidth = GetDevice()->GetWidth();
		devHeight = GetDevice()->GetHeight();

		if ( pt.x + width <= devWidth / 2 )		//put left
		{

		}
	}
}

/**
 * \brief 绘制对话框
 * 
 * 
 * 
 * \param fElapsedTime : 
 * \return 
 */
HRESULT CGuiFloatAccessKey::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( m_bPretendedHide )
	{
		m_x = -10000;
		m_y = -10000;
	}
	else
	{
		//Set pos follow chat select dlg
		if ( GetDevice()->GetWidth() == 800 )
		{
			if ( GetGameGuiManager()->m_guiChatSelect )
			{
				m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 270;
				m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 520 - GetHeight();

				if ( m_y > (GetDevice()->GetHeight() - 97 ) )
					m_y = GetDevice()->GetHeight()-97;
			}
			else
			{
				m_x = 400;
				m_y = 684 - GetHeight();
			}
		}
		else
		{
			if ( GetGameGuiManager()->m_guiChatSelect )
			{
				m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 422;
				m_y = GetGameGuiManager()->m_guiChatSelect->GetLocation().y + 684 - GetHeight();

				if ( m_y > (GetDevice()->GetHeight() - 97 ) )
					m_y = GetDevice()->GetHeight()-97;
			}
			else
			{
				m_x = 422;
				m_y = 684 - GetHeight();
			}
		}		
	}
	


	//Make sure this dlg is nearby float main dlg
	if ( GetGameGuiManager()->m_guiFloatControl )
	{
		stPointI pt;
		pt = GetGameGuiManager()->m_guiFloatControl->GetLocation();
		int width, height, devWidth, devHeight;
		width = GetGameGuiManager()->m_guiFloatControl->GetWidth();
		height = GetGameGuiManager()->m_guiFloatControl->GetHeight();

		devWidth = GetDevice()->GetWidth();
		devHeight = GetDevice()->GetHeight();

		if ( pt.x + width <= devWidth / 2 )		//put left
		{
			
		}
	}

	//Render dialog
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
		
	m_mouseOverAccessKeyNo = GetMouseOverAccessKeyNo();
	//Draw other
	hr &= RenderAccessKeyBtnDisableStatus( fElapsedTime );
	hr &= RenderAccessKeyNotes( fElapsedTime );
	hr &= RenderAccessKeyBtnToolTips( fElapsedTime );

	return hr;

	FUNCTION_END;
}

bool CGuiFloatAccessKey::OnFindAccessSkill()
{
	//if ( GetGameCursor()->IsDrag() && (GetGameCursor()->GetCursorImageType() == eCursorFindAccess) )
	{
		int id = GetMouseOverAccessKeyNo();
		if ( -1 != id )
		{
			enumGameAccelKeyType keyType;
			DWORD dwKeyID;
			BYTE curGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;
			BYTE key[4];
			key[0] = '0' + id;
			key[1] = 0;
			if( GetGameAccelKeyManager()->GetCurAccelKeyID(keyType,dwKeyID, curGroup, key) )
			{
				if ( GameAccelKeyType_Skill == keyType )
				{
					SkillBase_t* pSelectSkill = GetSkillBase( dwKeyID );
					//GetScene()->GetMainCharacter()->m_pActiveSkill = pSelectSkill;
					if ( GetGameGuiManager()->m_guiTeam )
						GetGameGuiManager()->m_guiTeam->m_pActiveSkill = pSelectSkill;
					if ( pSelectSkill && ( (pSelectSkill->dwType == SkillType_State) || (pSelectSkill->dwType == SkillType_StateAttack) )
						&& (pSelectSkill->dwTarget & SkillTarget_Single)
						&& !( (pSelectSkill->dwTarget & SkillTarget_Self) && !(pSelectSkill->dwTarget & SkillTarget_Single) ) )
					{
						if ( GetGameGuiManager()->m_guiTeam )
						{
							CCharacter* pCharacter = GetGameGuiManager()->m_guiTeam->GetSelMember();
							if ( pCharacter )
							{
								//if( GetScene()->GetMainCharacter()->bIsShopStart() )
								//{
								//	GetGameGuiManager()->AddClientSystemMessage("摆摊情况下不可以攻击");
								//	return false;
								//}

								CMainCharacter::stRunEvent runEvent;
								runEvent.id = pCharacter->GetID();
								runEvent.target = CMainCharacter::eTargetType_Character;
								runEvent.pSkill = pSelectSkill;
								runEvent.pt = pCharacter->GetPos();
								runEvent.speed = 0;
								runEvent.event = CMainCharacter::eEventType_Attack;

								GetScene()->GetMainCharacter()->SetRunEvent( runEvent );
								
								RestoreMouseIcon();
								return true;
							}
						}						
					}
				}
			}
		}
	}

	//RestoreMouseIcon();
	return false;
}

/**
 * \brief 对话框消息处理
 * 
 * 继承自基类
 * 
 * \param hWnd : 
 * \param uMsg : 
 * \param wParam : 
 * \param lParam : 
 * \return 
 */
bool CGuiFloatAccessKey::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			/*if ( GetGameCursor()->GetCursorImageType() == eCursorUseAccess )
			{
				if ( GetGameGuiManager()->m_guiTeam )
					GetGameGuiManager()->m_guiTeam->UseSkillOnTeamMember( false );
			}*/
		}
		break;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_DELETE:
			DeleteMouseOverAccessKey();
			break;
		default:
			break;
		}
		break;	
	default:
		break;
	}

	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}

/**
 * \brief 删除一个快捷键
 * 
 * 
 * 
 * \param key : 快捷键按钮id
 * \return 
 */
void CGuiFloatAccessKey::DeleteAccessKey( BYTE key )
{
	FUNCTION_BEGIN;

	RemoveAccessKeyBtnInfo( key - '0' );

	FUNCTION_END;
}

/**
 * \brief 删除快捷键所在按钮信息
 * 
 * 
 * 
 * \param key : 快捷键按钮id
 * \return 
 */
void CGuiFloatAccessKey::RemoveAccessKeyBtnInfo( BYTE key )
{
	FUNCTION_BEGIN;

	if ( key<0 || key >= FLOAT_ACCESS_KEY_NUM )
		return;

	if ( m_pBtnAccessKey[key] )
	{
		stResourceLocation rl;
		m_pBtnAccessKey[key]->SetBitmap( rl, 0 );
		m_pBtnAccessKey[key]->SetText("");
	}

	FUNCTION_END;
}

/**
 * \brief 删除鼠标所指快捷键
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::DeleteMouseOverAccessKey()
{
	FUNCTION_BEGIN;

	if ( (m_mouseOverAccessKeyNo <0) || (m_mouseOverAccessKeyNo >= FLOAT_ACCESS_KEY_NUM)	)
		return;

	//Really has a key to delete
	BYTE key[4];
	key[0] = '0'+m_mouseOverAccessKeyNo;
	key[1] = 0;

	if ( GetGameAccelKeyManager()->DeleteOneAccelKey( key, (BYTE)FLOAT_ACCESS_KEY_GROUP ) )
	{
		RemoveAccessKeyBtnInfo( (BYTE)m_mouseOverAccessKeyNo );
	}

	FUNCTION_END;
}

/**
 * \brief 刷新快捷键列表
 * 
 * 
 * 
 * \return 
 */
void CGuiFloatAccessKey::RefreshKeyGroupState()
{
	FUNCTION_BEGIN;

	if ( NULL == GetGameGuiManager()->m_guiSkill )
		return;
	//Update access key btn icon
	for( int i=0;i<FLOAT_ACCESS_KEY_NUM;i++ )
	{
		DWORD id;
		enumGameAccelKeyType type;
		BYTE key[4];
		key[0] = '0'+i;
		key[1] = 0;
		
		m_pBtnAccessKey[i]->SetText("");

		stResourceLocation rl;
		if ( GetGameAccelKeyManager()->GetAccessKeyIdAndType( key, (BYTE)FLOAT_ACCESS_KEY_GROUP, type, id ) )
		{
			switch( type )
			{
			case GameAccelKeyType_Skill:
				{
					if ( GetGameGuiManager()->m_guiSkill->GetSkillIconResourceFromID( id, rl ) )
					{
						m_pBtnAccessKey[i]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[i]->SetBitmap( rl, 3, dwState );
					}
					else
					{
						m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
					}
				}
				break;
			case GameAccelKeyType_TaoZhuang:
				{
					int fNum = 0;
					CGuiButton* btn = GetGameGuiManager()->m_guiUserInfo->getTaoZhuangButton();
					if ( btn->GetBitmapInfo( rl, fNum ) )
					{
						int nIndex = id;

						m_pBtnAccessKey[i]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[i]->SetBitmap( rl, 3, dwState );

						//sky 字体显示
						m_pBtnAccessKey[i]->SetTextColor(COLOR_ARGB(255,255,0,0));
						std::string strName = GetGameGuiManager()->m_guiUserInfo->getTaoZhuang_SName(nIndex);
						m_pBtnAccessKey[i]->SetText(strName.c_str());
						m_pBtnAccessKey[i]->SetFont(2);
					}
					else
					{
						m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
						m_pBtnAccessKey[i]->SetText(""); //sky
					}
				}
				break;
			case GameAccelKeyType_AttPak:
				{
					if ( GetGameGuiManager()->m_guiMain->GetResourceLocation( id, rl ) )
					{
						m_pBtnAccessKey[i]->SetBitmapScale( skillIconScale );
						m_pBtnAccessKey[i]->SetBitmap( rl, 3, dwState );
					}
					else
					{
						m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
					}
				}
				break;
			case GameAccelKeyType_Item:
			case GameAccelKeyType_Equipment:
				{
					CRoleItem* pItem = NULL;
					if ( type == GameAccelKeyType_Item )
					{
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );
						if ( (NULL == pItem) || !pItem->IsAccessNeedThisID() )
							pItem = GetScene()->GetMainCharacter()->FindItemByConstID( id );						
					}
					else if( type == GameAccelKeyType_Equipment )
						pItem = GetScene()->GetMainCharacter()->FindItemByID( id );

					if ( pItem )
					{
						rl = GetItemPicLocation( pItem->GetObject() );
						m_pBtnAccessKey[i]->SetBitmapScale( itemIconScale );
						m_pBtnAccessKey[i]->SetBitmap( rl, 1 );
					}
					else
					{
						m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
						m_pBtnAccessKey[i]->GetToolTips()->Clear();
					}
				}
				break;
			case GameAccelKeyType_Face:
				{
					if ( GetEmotionInfo()->GetResourceLocation( id, rl ) )
					{
						__SetEmotionAccessBtn(rl,i);
					}
					else
					{
						m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
					}
				}
				break;

			default:
				break;
			}
		}
		else		//Cur btn has no access key
		{
			m_pBtnAccessKey[i]->SetBitmap( rl, 0 );
		}

		if ( m_pBtnAccessKey[i]->GetToolTips() )
			m_pBtnAccessKey[i]->GetToolTips()->Clear();

	}

	FUNCTION_END;
}

/**
 * \brief 设置新的快捷键
 * 
 * 
 * 
 * \param keyID : 快捷键按钮id
 * \return 成功则返回true, 否则返回false
 */
bool CGuiFloatAccessKey::SetAccessKey( const int& keyID )
{
	FUNCTION_BEGIN;

	if ( (keyID >= FLOAT_ACCESS_KEY_NUM) || (keyID < 0) )
	{
		RestoreMouseIcon();
		return false;
	}

	if( NULL == m_pBtnAccessKey[keyID] )
	{
		RestoreMouseIcon();
		return false;
	}

	if ( NULL == GetGameGuiManager()->m_guiMain )
		return false;
	
	CGuiMain::enumActiveAccessType type;
	if ( !(GetGameGuiManager()->m_guiMain->IsAnyActiveForAccessKeySetting( type ) ) )
	{
		RestoreMouseIcon();
		return false;
	}

	if ( keyID != 0 )
		m_pBtnAccessKey[keyID]->SetLocation( g_nFloatAccessBtnLeft+(keyID-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
	else
		m_pBtnAccessKey[keyID]->SetLocation( g_nFloatAccessBtnLeft+(FLOAT_ACCESS_KEY_NUM-1)*g_FloatAccessBtnSpace, g_nFloatAccessBtnTop );
	stResourceLocation rl;
	switch( type )
	{
	case CGuiMain::ActiveAccessSkill:
		{
			if ( GetGameGuiManager()->m_guiSkill->GetActiveSkillIconResource( rl ) )
			{
				m_pBtnAccessKey[keyID]->SetBitmapScale( skillIconScale );
				m_pBtnAccessKey[keyID]->SetBitmap( rl, 3, dwState );

				//Save access key change
				stGameAccelKey key;
				key.type = GameAccelKeyType_Skill;
				key.dwID = GetGameGuiManager()->m_guiSkill->GetActiveSkillIconID();
				key.szAccelKey[0] = '0' + keyID;
				key.szAccelKey[1] = 0;
				key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

				GetGameAccelKeyManager()->SetKey(key);
				GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

				//Restore mouse icon as default
				GetGameGuiManager()->m_guiSkill->SetActiveSkillIcon();
				RestoreMouseIcon();

				return true;
			}
		}
		break;
	case CGuiMain::ActiveAccessTaoZhuang: //sky 换装放入物品栏
		{
			m_pBtnAccessKey[keyID]->SetText("");  //sky 字体清除防叠加
			int fNum = 0;
			CGuiButton* btn = GetGameGuiManager()->m_guiUserInfo->getActiveIcon();
			if ( btn->GetBitmapInfo( rl, fNum ) )
			{
				m_pBtnAccessKey[keyID]->SetBitmapScale( skillIconScale );
				m_pBtnAccessKey[keyID]->SetBitmap( rl, 3, dwState );
				
                m_pBtnAccessKey[keyID]->SetTextColor(COLOR_ARGB(255,255,0,0));
				m_pBtnAccessKey[keyID]->SetText(btn->GetText());
				m_pBtnAccessKey[keyID]->SetFont(2);

				//Save access key change
				stGameAccelKey key;
				key.type = GameAccelKeyType_TaoZhuang;
				key.dwID = GetGameGuiManager()->m_guiUserInfo->getActiveIndex();
				key.szAccelKey[0] = '0' + keyID;
				key.szAccelKey[1] = 0;
				key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

				GetGameAccelKeyManager()->SetKey(key);
				GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

				//Restore mouse icon as default
				GetGameGuiManager()->m_guiUserInfo->setActiveIcon(NULL);
				RestoreMouseIcon();

				return true;
			}
		}
		break;
	case CGuiMain::ActiveAccessAttaPak:
		{
		    int index = GetScene()->GetMainCharacter()->GetPKMode() +1;
			if ( GetGameGuiManager()->m_guiMain->GetResourceLocation( index, rl ) )
			{
				m_pBtnAccessKey[keyID]->SetBitmapScale( skillIconScale );
				m_pBtnAccessKey[keyID]->SetBitmap( rl, 3, dwState );

				//Save access key change
				stGameAccelKey key;
				key.type = GameAccelKeyType_AttPak;
				key.dwID =  GetScene()->GetMainCharacter()->GetPKMode() + 1;
				key.szAccelKey[0] = '0' + keyID;
				key.szAccelKey[1] = 0;
				key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

				GetGameAccelKeyManager()->SetKey(key);
				GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

				//Restore mouse icon as default
				GetGameGuiManager()->m_guiMain->set2ActiveIcon(NULL);
				RestoreMouseIcon();

				return true;
			}
		}
		break;
	case CGuiMain::ActiveAccessItem:
	case CGuiMain::ActiveAccessEquipment:
		{
			if ( GetFloatItemResource( rl ) )
			{
				m_pBtnAccessKey[keyID]->SetBitmapScale( itemIconScale );
				m_pBtnAccessKey[keyID]->SetBitmap( rl, 1 );

				//Save access key change
				stGameAccelKey key;
				key.type = (type == CGuiMain::ActiveAccessItem ) ? GameAccelKeyType_Item : GameAccelKeyType_Equipment;
				key.dwID = GetFloatItemThisID(type == CGuiMain::ActiveAccessEquipment);
				key.szAccelKey[0] = '0' + keyID;
				key.szAccelKey[1] = 0;
				key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;

				GetGameAccelKeyManager()->SetKey(key);
				GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP );

				//Move back float item
				MoveBackFloatItem();
				RestoreMouseIcon();

				return true;
			}
		}
		break;
	case CGuiMain::ActiveAccessEmotion:
		{
			ICON_ACCESS_COME_FROM _from = GetEmotionInfo()->GetComeFrom();
			if( FROM_FLOAT == _from || FROM_MAIN == _from )
			{
				if ( GetGameAccelKeyManager() && GetGameAccelKeyManager()->m_curEditAccessKey.IsValid() )
				{
					EditAccessKeyBtn(keyID);
				}
			}			
			else  if( FROM_EMOTION == _from)
			{
				if( GetEmotionInfo()->GetActiveResourceLocation(rl) )
				{
					__SetEmotionAccessBtn(rl,keyID);
				
					//Save access key change
					stGameAccelKey key;
					key.type = GameAccelKeyType_Face;
					key.dwID = GetEmotionInfo()->GetActiveEmotionIcon();
					key.szAccelKey[0] = '0' + keyID;
					key.szAccelKey[1] = 0;
					key.mGroup = (BYTE)FLOAT_ACCESS_KEY_GROUP;  // must be FLOAT_ACCESS_KEY_GROUP ,but not EMOTION_ACCESS_KEY_GROUP

					GetGameAccelKeyManager()->SetKey(key);
					GetGameAccelKeyManager()->Save( FLOAT_ACCESS_KEY_GROUP);	  

					//Restore mouse icon as default
					GetEmotionInfo()->ClearActiveEmotionIcon();
					RestoreMouseIcon();

				}
			}
					
		}
		break;
	default:
		break;
	}

	RestoreMouseIcon();
	return false;

	FUNCTION_END;
}