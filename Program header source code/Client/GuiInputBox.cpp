/**
*\file		GuiInputBox.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    输入对话框
*
* 
*
*/


#include "public.h"
#include "./GameGuiManager.h"
#include ".\guiinputbox.h"
#include "game_script.h"
#include "./RoleItem.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiPrivateChat.h"
#include "./GuiWorldConn.h"
#include "./GuiShopDialog.h"
#include "./GuiSendMailDialog.h"
#include "./CountryInfo.h"
#include "./GuiSelectSeptDareType.h"
#include "../luabind/public.h"
#include "./GuiNpcDialog.h"
#include ".\guiitem.h"
#include "GuiUserInfo.h"
void *pComboItem=NULL;
void CGuiInputBox::bind_lua(lua_State* L)
{
	using namespace luabind;
	CGuiDialog::bind_lua(L);
	module(L)
	[
		class_<CGuiInputBox,CGuiDialog>("CGuiInputBox")
			.def("OnInputBoxEnter",&CGuiInputBox::OnInputBoxEnter)
	];
	m_lua->SetGlobal("this",this);
}

/**
 * \brief 构造函数
 * 
 * 构造函数
 * 
 * \return 无
 */
CGuiInputBox::CGuiInputBox(const char* pszText,enumGuiActionType type,void* pData)
: m_strText(pszText)
, m_action(type)
, m_pData(pData)
, m_pEditBox(NULL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

/**
 * \brief 关闭对话框
 * 
 * 关闭对话框
 * 
 * \return 无
 */
void CGuiInputBox::OnClose(void)
{
	FUNCTION_BEGIN;

	if(g_pMoneyInputBox == this)
		g_pMoneyInputBox = NULL;

	FUNCTION_END;
}

/**
 * \brief 对话框创建
 * 
 * 对话框创建
 * 
 * \return 无
 */
void CGuiInputBox::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	CGuiStatic* pText = GetStatic(1);
	Assert(pText);
	pText->SetText(m_strText.c_str());
	m_pEditBox = GetEditBox(2);

	GetGuiManager()->RequestFocus(this);
	this->SetFocus((CGuiControl*)m_pEditBox);

	FUNCTION_END;
}

/**
 * \brief 确定按钮
 * 
 * 确定按钮
 * 
 * \param bOk : 是否是确定按钮
 * \return 无
 */
void CGuiInputBox::OnInputBoxEnter(bool bOk)
{
	FUNCTION_BEGIN;

	CGuiInputBox* pInput = (CGuiInputBox*)this;
	if(!pInput) return;
	switch(pInput->m_action)
	{
	case eAction_SplitItem:		// 分裂物品
		{
			if(bOk)
			{
				stSplitItemData* st = (stSplitItemData*)pInput->m_pData;
				Assert(st);
				CRoleItem* pSrcItem = GetScene()->GetMainCharacter()->FindItemByID(st->qwThisID);
				DWORD dwNum = strtoul(pInput->m_pEditBox->GetText(),NULL,10);
				if( dwNum && pSrcItem && dwNum <= pSrcItem->GetCount() )
				{
					// 如果数量足够才能分裂
					CRoleItem* pDstItem = GetScene()->GetMainCharacter()->FindItemByLocation(st->dst);
					if( !pSrcItem->IsMoney() && !pDstItem )
					{
						// 如果目标点没东西才能分裂
						CGuiItemCell* pCell;
						int count;
						pSrcItem->GetTrueCell(pCell,count);
						pSrcItem->AddShowLocation(pCell,dwNum,st->dst,dwNum == count);
					}
				}
			}
			pInput->Close();
		}
		CRoleItem::s_bCanMoveItem = true;
		break;
	case eAction_World:
		{
			
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText == NULL || szText[0] == 0 || szText[0] == ' ' || szText[strlen(szText)-1] == ' ')
				{
					GameMessageBox("输入不合法！");
					return;
				}
				//Limit union & sept name length to 16;
				if ( strlen( szText ) > 16 )
				{
					GameMessageBox( "输入名字长度不得超过16个字符!" );
					return;
				}

				if(szText[0])
				{
					switch((long)(pInput->m_pData)) {
					case 0:  //建立行会
						{
							stCreateUnionCmd  cmd;
							strncpy(cmd.UnionName,szText,sizeof(cmd.UnionName));
							SEND_USER_CMD(cmd);
						}
						break;
					case 1:  //开除帮会成员
						{
							stFireMemberFromUnionCmd cmd;
							strncpy(cmd.memberName,szText,sizeof(cmd.memberName));
							SEND_USER_CMD(cmd);
						}
						break;
					case 2:  //邀请帮会成员
						{
							AddTong(szText);
						}
						break;
					case 3:  //邀请加入好友
						{
							AddFriend(szText);

						}
						break;
					case 4: //加入黑名单
						{
							AddBad(szText);
						}
						break;
					case 5: //建立门派
						{
							stCreateSchoolCmd cmd;
							strncpy(cmd.SchoolName,szText,sizeof(cmd.SchoolName));
							SEND_USER_CMD(cmd);
						}
						break;
					case 6:  //收徒弟
						{
							AddPrentice(szText);
						}
						break;
					case 7: //逐出师门
						{
							stFireMemberFromSchoolCmd cmd;
							strncpy(cmd.memberName,szText,sizeof(cmd.memberName));
							SEND_USER_CMD(cmd);
						}
						break;
					case 8: //加入家族会员
						{
							AddSept(szText);
						}
						break;
					case 9: //建立家族
						{
							stCreateSeptCmd  cmd;
							strncpy(cmd.SeptName,szText,sizeof(cmd.SeptName));
							SEND_USER_CMD(cmd);
						}
						break;
					}
				}
				
			}
			pInput->Close();
		}
		break;
	case eAction_TongChangeName:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText == NULL || szText[0] == 0 || szText[0] == ' ' || szText[strlen(szText)-1] == ' ')
				{
					GameMessageBox("输入不合法！");
					return;
				}
				if(szText[0])
				{
					stUnionRember* pRember = (stUnionRember*) m_pData;
					if (m_pData)
					{
						stChangeUnionMemberAliasName cmd;
						strncpy(cmd.name, pRember->memberName,sizeof(cmd.name));
						strncpy(cmd.aliasname, szText,sizeof(cmd.name));
						SEND_USER_CMD(cmd);
					}
				}
			}
			pInput->Close();
		}
		break;
	case eAction_SeptChangeName:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText == NULL || szText[0] == 0 || szText[0] == ' ' || szText[strlen(szText)-1] == ' ')
				{
					GameMessageBox("输入不合法！");
					return;
				}
				if(szText[0])
				{
					stSeptRember* pRember = (stSeptRember*) m_pData;
					if (m_pData)
					{
						stChangeSeptMemberAliasName cmd;
						strncpy(cmd.name, pRember->memberName,sizeof(cmd.name));
						strncpy(cmd.aliasname, szText,sizeof(cmd.name));
						SEND_USER_CMD(cmd);
					}
				}
			}
			pInput->Close();
		}
		break;
	case eAction_BuyItem:
		{
			if(bOk)
			{
				stObjectLocation * pos = (stObjectLocation*)pInput->m_pData;
				DWORD dwNum = strtoul(pInput->m_pEditBox->GetText(),NULL,10);
				if (dwNum==0)
				{
					GameMessageBox("输入的购买数量错误！");
					break;
				}
					
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
				if(pItem)
				{
					if (pItem->GetObjectBase()->dwMaxCount<dwNum)
					{
						char szTxt[64]={0};
						sprintf(szTxt,"%s 一次购买的最大个数为 %d!",
							pItem->GetObjectName(),pItem->GetObjectBase()->dwMaxCount);
						GameMessageBox(szTxt);
						break;
					}
					stBuyObjectNpcTradeUserCmd cmd;
					cmd.dwObjectID = pItem->GetObjectID();
					cmd.dwNum = dwNum;
					cmd.nLevel = pItem->GetUpGrade();
					SEND_USER_CMD(cmd);
				}
			}
			if (GetGameGuiManager()->m_guiNpcDialog)
				GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			pInput->Close();
		}
		break;
		case eAction_SalaryBuyItem: //soke 2016 - 3 - 10 工资购买
		{
			if(bOk)
			{
				stObjectLocation * pos = (stObjectLocation*)pInput->m_pData;
				DWORD dwNum = strtoul(pInput->m_pEditBox->GetText(),NULL,10);
				if (dwNum==0)
				{
					GameMessageBox("输入的购买数量错误！");
					break;
				}
					
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
				if(pItem)
				{
					if (pItem->GetObjectBase()->dwMaxCount<dwNum)
					{
						char szTxt[64]={0};
						sprintf(szTxt,"%s 一次购买的最大个数为 %d!",
							pItem->GetObjectName(),pItem->GetObjectBase()->dwMaxCount);
						GameMessageBox(szTxt);
						break;
					}
					stBuyObjectSalaryTradeUserCmd cmd;
					cmd.dwObjectID = pItem->GetObjectID();
					cmd.dwNum = dwNum;
					cmd.nLevel = pItem->GetUpGrade();
					SEND_USER_CMD(cmd);
				}
			}
			if (GetGameGuiManager()->m_guiNpcDialog)
		        GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			pInput->Close();
		}
		break;
	case eAction_TicketBuyItem: //soke 2023 - 7 - 31 积分点数购买
		{
			if(bOk)
			{
				stObjectLocation * pos = (stObjectLocation*)pInput->m_pData;
				DWORD dwNum = strtoul(pInput->m_pEditBox->GetText(),NULL,10);
				if (dwNum==0)
				{
					GameMessageBox("输入的购买数量错误！");
					break;
				}
					
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
				if(pItem)
				{
					if (pItem->GetObjectBase()->dwMaxCount<dwNum)
					{
						char szTxt[64]={0};
						sprintf(szTxt,"%s 一次购买的最大个数为 %d!",
							pItem->GetObjectName(),pItem->GetObjectBase()->dwMaxCount);
						GameMessageBox(szTxt);
						break;
					}
					stBuyObjectTicketTradeUserCmd cmd;
					cmd.dwObjectID = pItem->GetObjectID();
					cmd.dwNum = dwNum;
					cmd.nLevel = pItem->GetUpGrade();
					SEND_USER_CMD(cmd);
				}
			}
			if (GetGameGuiManager()->m_guiNpcDialog)
		       GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			pInput->Close();
		}
		break;
	////////////////////////////////////////////////////////////////	
	case eAction_ChongzhiBuyItem: //soke 2023 - 10 - 10 充值点数购买
		{
			if(bOk)
			{
				stObjectLocation * pos = (stObjectLocation*)pInput->m_pData;
				DWORD dwNum = strtoul(pInput->m_pEditBox->GetText(),NULL,10);
				if (dwNum==0)
				{
					GameMessageBox("输入的购买数量错误！");
					break;
				}
					
				CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
				if(pItem)
				{
					if (pItem->GetObjectBase()->dwMaxCount<dwNum)
					{
						char szTxt[64]={0};
						sprintf(szTxt,"%s 一次购买的最大个数为 %d!",
							pItem->GetObjectName(),pItem->GetObjectBase()->dwMaxCount);
						GameMessageBox(szTxt);
						break;
					}
					stBuyObjectChongzhiTradeUserCmd cmd;
					cmd.dwObjectID = pItem->GetObjectID();
					cmd.dwNum = dwNum;
					cmd.nLevel = pItem->GetUpGrade();
					SEND_USER_CMD(cmd);
				}
			}
			if (GetGameGuiManager()->m_guiNpcDialog)
		       GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			pInput->Close();
		}
		break;	
	//////////////////////////////////////////////////////////////////////		
	case eAction_TongAttack:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText[0])
				{
					stActiveDareCmd  cmd;
					sprintf(cmd.name,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.name));
					sprintf(cmd.fromRelationName,
						GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName,sizeof(cmd.fromRelationName));
					sprintf(cmd.toRelationName,szText,sizeof(cmd.toRelationName));
					cmd.dwDareType = UNION_DARE;

					SEND_USER_CMD(cmd);
				}
			}
			pInput->Close();
		}
		break;

	case eAction_SeptAttack:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText[0])
				{
					stActiveDareCmd  cmd;
					sprintf(cmd.name,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.name));
					sprintf(cmd.fromRelationName,
						GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName,sizeof(cmd.fromRelationName));
					sprintf(cmd.toRelationName,szText,sizeof(cmd.toRelationName));
					cmd.dwDareType = SEPT_DARE;
					cmd.byDareRepute = 0;

					//backup cmd
					GetGameGuiManager()->AddSelectSeptDareTypeDlg();
					if ( GetGameGuiManager()->m_guiSelectSeptDareTypeDlg )
					{
						GetGameGuiManager()->m_guiSelectSeptDareTypeDlg->m_cmdSeptDare = cmd;
					}

					//SEND_USER_CMD(cmd);
				}
			}
			pInput->Close();
		}
		break;

	case eAction_SchoolAttack:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(szText[0])
				{
					stActiveDareCmd  cmd;
					sprintf(cmd.name,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.name));
					sprintf(cmd.fromRelationName,
						GetGameGuiManager()->m_guiWorldConn->m_SchoolInfo.schoolName,sizeof(cmd.fromRelationName));
					sprintf(cmd.toRelationName,szText,sizeof(cmd.toRelationName));
					cmd.dwDareType = SCHOOL_DARE;

					SEND_USER_CMD(cmd);
				}
			}
			pInput->Close();
		}
		break;
	case eAction_StartShop:
		{
			if(bOk)
			{
				const char* szText = pInput->m_pEditBox->GetText();
				if(	NULL != szText && strlen(szText) > 0 )
				{
					GetScene()->GetMainCharacter()->SetMainRoleShopAdv(szText);
				}
			}
			pInput->Close();
		}
		break;
	case eAction_Recommend:
		{
			if( bOk)
			{
			}
			else
			{}
			pInput->Close();
		}
		break;
	case eAction_Add_Property: //soke 属性自由加点弹窗
		{
			if ( bOk )
			{
				int type = (int)m_pData;
				const char* pText = pInput->m_pEditBox->GetText();
				int nCount = atoi( pText );
				if ( GetGameGuiManager()->m_guiUserInfo )
				{
					GetGameGuiManager()->m_guiUserInfo->SendAddUserProperty( type , nCount );
				}
			}
			pInput->Close();
		}
		break;
	case eAction_StopOneTalk:
		{
			if( bOk)
			{
			     const char* pText = pInput->m_pEditBox->GetText();
			   
			     size_t len = strlen(pText); 
			     char *szTemp = (char*)_alloca(sizeof(char) * (len + 1));
			     memcpy(szTemp,pText,len);
			     szTemp[len] = 0;

			     str_trim_right(szTemp);
			     str_trim_left(szTemp);
 

			     if( NULL != szTemp && strlen(szTemp) > 0 )
			     {
				     stForbidTalkCountryUserCmd  cmd;
				     sprintf(cmd.name,szTemp,sizeof(cmd.name));
				     cmd.name[sizeof(cmd.name)-1] = 0;
				     SEND_USER_CMD(cmd);
			     }
			     
			}
		
			pInput->Close();
		}
		break;
	case eAction_ChangeArmyName:
		{
			if(bOk)
			{
				const char* pText = pInput->m_pEditBox->GetText();

				size_t len = strlen(pText); 
				char *szTemp = (char*)_alloca(sizeof(char) * (len + 1));
				memcpy(szTemp,pText,len);
				szTemp[len] = 0;

				str_trim_right(szTemp);
				str_trim_left(szTemp);


				if( NULL != szTemp && strlen(szTemp) > 0 )
				{
					stChangeArmyNameUserCmd  cmd;
					cmd.dwArmyID = (DWORD)m_pData;
					sprintf(cmd.newArmyName,szTemp,sizeof(cmd.newArmyName));
					cmd.newArmyName[sizeof(cmd.newArmyName)-1] = 0;
					SEND_USER_CMD(cmd);
				}
			}
			if(CArmyManager::GetSingleton()->m_guiArmyListDlg)
				CArmyManager::GetSingleton()->m_guiArmyListDlg->Close();
			Close();
		}
		break;
	case eAction_RecruitArmy:  // 将军招收队长
		{
			if(bOk)
			{
				const char* pText = pInput->m_pEditBox->GetText();

				size_t len = strlen(pText); 
				char *szTemp = (char*)_alloca(sizeof(char) * (len + 1));
				memcpy(szTemp,pText,len);
				szTemp[len] = 0;

				str_trim_right(szTemp);
				str_trim_left(szTemp);


				if( NULL != szTemp && strlen(szTemp) > 0 )
				{
					stAddArmyCaptainUserCmd  cmd;
					strcpy(cmd.capName,szTemp);
					SEND_USER_CMD(cmd);
				}
			}
			//if(CArmyManager::GetSingleton()->m_guiArmyItemDlg)
			//	CArmyManager::GetSingleton()->m_guiArmyItemDlg->Close();
			Close();
		}
		break;
		//Add By David Fro 2006-3-20
	case eAction_PrisonCountry:  // 关玩家
		{
			if( bOk )
			{
				//
				const char* pCharName = pInput->m_pEditBox->GetText();
				int len = strlen(pCharName); 
				char *szTemp = (char*)_alloca(sizeof(char) * (len + 1));
				memcpy(szTemp,pCharName,len);
				szTemp[len] = 0;
				str_trim_right(szTemp);
				str_trim_left(szTemp);
				if( NULL != szTemp && strlen(szTemp) > 0 )
				{
					stSendPrisonCountryCmd  cmd;
					strcpy(cmd.name,szTemp);
					SEND_USER_CMD(cmd);
				}				
			}
			//
			Close();
		}
		break;
	case eAction_NpcDlgInput:
		if( bOk )
		{
			int type = (int)m_pData;
			const char* pszName = GetInputText();
			if( NULL != pszName && strlen(pszName) > 0 )
			{
				OnNpcDialogInput(type,pszName);
			}
			
		}
		Close();
		//GetGameGuiManager()->EndNpcDialog();
		break;
	case eAction_RenameEquip: //sky 一键换装命名
		{
			if (bOk)
			{
				const char* pText = pInput->m_pEditBox->GetText();
				if (pText == NULL || pText[0] == 0 || pText[0] == ' ' || pText[strlen(pText) - 1] == ' ')
				{
					GameMessageBox("输入不合法！");
					return;
				}
				//Limit union & sept name length to 7;
				if (strlen(pText) > 7)
				{
					GameMessageBox("输入名字长度不得超过7个字符!");
					return;
				}
				if (pComboItem)
				{
					CGuiComboBox* newpComboItem = (CGuiComboBox*)pComboItem;
					if (newpComboItem)
					{
						GuiComboBoxItem* boxitem = newpComboItem->GetSelectedItem();
						if (boxitem)
						{
							//命名前加.
							sprintf(boxitem->strText, "%d.%s", newpComboItem->GetCurItem()+1,pText);
							//strncpy(boxitem->strText, pText, 256);
							newpComboItem->SetText(boxitem->strText);
							newpComboItem->SetWindowTextA(boxitem->strText);
						}
					}
				}
			}
			pInput->Close();
		}
		break;
	case eAction_Lua_InputBox:
		{
			if( bOk )
			{	
				const char* pText = pInput->m_pEditBox->GetText();	
				if(pText[0])
				{
					const char* funname = (char*)pInput->m_pData;
					Engine_WriteLogF("luainpubox:%s,%s\n", pText,funname);
					stLuaInputBoxUserCmd cmd; 
					strcpy(cmd.funName,funname);
					strcpy(cmd.text,pText);
					Engine_WriteLogF("luainpubox:%s,%s\n", cmd.text,cmd.funName);
					SEND_USER_CMD(cmd);
				}
				
			}
			Close();
		}
		break;
	}

	FUNCTION_END;
}

void CGuiInputBox::OnNpcDialogInput(int type,const char* pszStr)
{
	switch( type )
	{
	case 1:		//皇帝禁言
		{
			stEmperorPunishCountryCmd cmd;
			strcpy(cmd.name,pszStr);
			cmd.method = 1;	//皇帝禁言
			SEND_USER_CMD(cmd);
		}
		break;
	case 2:		//皇帝关**监狱
		{
			stEmperorPunishCountryCmd cmd;
			strcpy(cmd.name,pszStr);
			cmd.method = 2;	//皇帝关**监狱
			SEND_USER_CMD(cmd);
		}
		break;
	case 3://soke 2016 -3 -11 任命外交官（停用）
		{
			stAppointDiplomatCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 5://soke 2016 -3 -11 城主任命巡捕(停用)
		{
			stAppointCityCatcherCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;

	case 7:  //soke 2016 -3 -11 国王任命巡捕（左上）
		{
			stAppointCatcherCmd  cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 9:  //soke 2016 -3 -11 国王任命巡捕（左下）
		{
			stAppointCatchexCmd  cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 11://soke 2016 -3 -11 国王任命御史大夫（右上）
		{
			stAppointCensorCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 13://soke 2016 -3 -11 国王任命御史大夫（右下）
		{
			stAppointCensosCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 15://soke 2016 -3 -11 国王任命元帅
		{
			stAppointYuanshuaiCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	case 17://soke 2016 -3 -11 国王任命宰相
		{
			stAppointZaixiangCmd cmd;
			strncpy(cmd.name,pszStr,sizeof(cmd.name));
			SEND_USER_CMD(cmd);
		}
		break;
	}
}
/**
 * \brief 注册脚本函数
 * 
 * 注册脚本函数
 * 
 * \return 无
 */
void CGuiInputBox::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	FUNCTION_END;
}

void CGuiInputBox::SetNumber(int minNumber,int maxNumber)
{
	FUNCTION_BEGIN;

	m_pEditBox->SetMinNumber(minNumber);
	m_pEditBox->SetMaxNumber(maxNumber);
	m_pEditBox->SetStyle(CGuiEditBox::GUI_ES_NUMBER);

	FUNCTION_END;
}

void CGuiInputBox::SetLengthOfText(int iLength)
{
	m_pEditBox->SetMaxCharCount(iLength);
}

void CGuiInputBox::SetDefault(const char* pszDefault)
{
	FUNCTION_BEGIN;

	m_pEditBox->SetText(pszDefault);

	FUNCTION_END;
}

const char* CGuiInputBox::GetInputText()
{
	FUNCTION_BEGIN;
	memset(m_szInputData,0,512);
	const char* pszStr = m_pEditBox->GetText();
	strncpy(m_szInputData,pszStr,strlen(pszStr) + 1);	
	str_trim_right(m_szInputData);
	str_trim_left(m_szInputData);
	return m_szInputData;
}