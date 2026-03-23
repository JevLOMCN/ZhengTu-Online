#include "./public.h"
#include <boost/regex.hpp>
#include "./GameGuiManager.h"
#include ".\guiitemmake.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiInputBox.h"
#include "./GameState.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiNpcDialog.h"
#include "./GuiItem.h"


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObjectBase : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stResourceLocation GetObjectBasePicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);
	
	rl.group = c_nGroupItemCommon;
	rl.frame = pObjectBase->dwItemPic;
	
	return rl;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObjectBase : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stResourceLocation GetObjectBasePatternPicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);
	
	rl.group = c_nGroupItemCommonPattern;
	rl.frame = pObjectBase->dwItemPic;
	
	return rl;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObjectBase : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stResourceLocation GetObjectBasePattern2PicLocation(ObjectBase_t* pObjectBase)
{
	FUNCTION_BEGIN;

	Assert(pObjectBase);
	stResourceLocation rl;
	if(!pObjectBase) return rl;
	rl.SetFileName(c_szItemGrpPack);
	
	rl.group = c_nGroupItemCommonPattern2;
	rl.frame = pObjectBase->dwItemPic;
	
	return rl;

	FUNCTION_END;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param aObjects : ÃèÊö
 * \param pszString : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
template < class tVecObject >
void ParseMakeString(tVecObject & aObjects,const char* pszString)
{
	FUNCTION_BEGIN;

	SetFrameAllocator fa;
	std::vector<std::stack_string, std::stack_allocator<std::stack_string> > aParams1;

	_parse_str(aParams1,pszString,';');
	std::string strTemp;
	const char pszExp[] = "([^\\[]*)\\[([^\\]]*)\\]";
	boost::regex expression1(pszExp,boost::regbase::normal|boost::regbase::icase);
	for(size_t i=0;i<aParams1.size();++i)
	{
		std::vector<std::string > vResult;
		vResult.reserve(8);
		const char* pszTemp = aParams1[i].c_str();
		strTemp = pszTemp;

		boost::regex_split(std::back_inserter(vResult),strTemp,expression1);

		if(vResult.size() == 2)
		{
			pszTemp = vResult[1].c_str();
			std::vector<int,std::stack_allocator<int> > aParams2;
			if(stricmp("id",vResult[0].c_str() ) == 0)
			{
				// °´ ID ²éÕÒÎïÆ·

				_parse_str_num(aParams2,vResult[1].c_str());
				for( size_t k = 0; k < aParams2.size(); ++k )
				{
					ObjectBase_t* pObject = GetObjectBase(aParams2[k]);
					if(pObject)
					{
						aObjects.push_back(pObject);
					}
				}
			}
			else if(stricmp("type",vResult[0].c_str() ) == 0 )
			{
				// °´ ÀàĞÍ ²éÕÒÎïÆ·
				_parse_str_num(aParams2,vResult[1].c_str());

				for( size_t k = 0; k < aParams2.size(); ++k )
				{
					for(size_t h = 0 ; h < g_tableObjectBase.size();++h)
					{
						if(g_tableObjectBase[h].dwType == aParams2[k])
						{
							aObjects.push_back(&g_tableObjectBase[h]);
							break;
						}
					}
				}
			}
		}
	}

	FUNCTION_END;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemMake::CGuiItemMake(void)
{
	FUNCTION_BEGIN;

	m_bCloseIsHide = true;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemMake::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pLabel = GetStatic(1);
	m_pTable = GetTable(2);
	m_pTable->m_iTableType = OBJECTCELLTYPE_MAKE;

	m_pListBox=GetListBox(3);
	

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObject : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemMake::AddMakeObject(ObjectBase_t* pObject)
{
	FUNCTION_BEGIN;

	/*CGuiItemCell* pCell=m_pTable->AppendItem(pObject->cx,
		pObject->cy);
	

	LoadMakeImage(pObject,pCell,false);
	*/

	POINT ptCell = m_pTable->AppendItemPosition(pObject->cx,pObject->cy);

	stAddObjectPropertyUserCmd cmd;
	cmd.byActionType = EQUIPACTION_INIT;
	memset(&cmd.object,0,sizeof(cmd.object));
	strncpy(cmd.object.strName,pObject->strName,sizeof(cmd.object.strName));
	cmd.object.dwObjectID = pObject->dwID;
	cmd.object.qwThisID = INVALID_THISID;
	cmd.object.pos.dwLocation = OBJECTCELLTYPE_MAKE;
	cmd.object.pos.dwTableID = 0;
	cmd.object.pos.x = ptCell.x;
	cmd.object.pos.y = ptCell.y;
	GetScene()->GetMainCharacter()->AddItem(&cmd);


	if (ResetMake(pObject))
	{
		m_pListBox->AddItem(pObject->strName,pObject);
	}
	else
	{
		m_pListBox->AddItem(pObject->strName,pObject,COLOR_ARGB(255,255,0,0));
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param params : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemMake::SetParam(std::vector<std::string> & params)
{
	FUNCTION_BEGIN;

	if( params.size() > 0 )
	{
		CMainCharacter* pMainRole = GetScene()->GetMainCharacter();

		ParseMakeString(m_aMakeObject,params[0].c_str());
		for(size_t i=0; i < m_aMakeObject.size(); ++i)
		{
			AddMakeObject(m_aMakeObject[i]);
		}
	}

	if (m_pListBox->GetCount()>0)
	{
		m_pListBox->SetCurItem(0);
		OnGuiEvent(EVENT_LISTBOX_SELECTION,3,NULL);
	}

	GetGameState()->SetStateItem(eGameStateItem_Up);



	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemMake::OnClose(void)
{
	FUNCTION_BEGIN;


//	GetGameGuiManager()->m_guiItemMake = NULL;
	GetGameState()->SetStateItem(eGameStateItem_NpcDialog);
	
	GetGameGuiManager()->OnNpcActionEnd();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nEvent : ÃèÊö
 * \param nID : ÃèÊö
 * \param pControl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiItemMake::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 4:	// 
			{
				if (m_pListBox->GetCount()>0)
				{
					ObjectBase_t* pObject=(ObjectBase_t*)m_pListBox->GetItemData(m_pListBox->GetCurItem());
					
					if (pObject)
					{
						stFoundItemPropertyUserCmd stMakeCmd;
						stMakeCmd.dwObjectID=pObject->dwID;
						SEND_USER_CMD(stMakeCmd);
					}
				}
				OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
			}
			break;

		case -1:
			{
				if(GetGameGuiManager()->m_guiItem)
				{
					GetGameGuiManager()->m_guiItem->SetVisible(false);
				}

				GetGameGuiManager()->OnNpcActionEnd();
			}

		}
	case EVENT_LISTBOX_SELECTION:
		switch(nID) 
		{
		case 3:
			{
				ResetMake((ObjectBase_t* )m_pListBox->GetItemData(m_pListBox->GetCurItem()));
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiItemMake::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	return hr;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObject : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
	FUNCTION_END;
}
bool CGuiItemMake::ResetMake(ObjectBase_t* pObject)
{
	FUNCTION_BEGIN;

	bool      bNeed=true;

	if (pObject)
	{

		Assert(pObject);
		
		
		SetFrameAllocator fa;
		std::vector<UINT, std::stack_allocator<UINT> > aid;
		aid.reserve(16);
		
		std::string strParse=pObject->strField36;
		_parse_str_num(aid,strParse.data());
		
		DWORD nObjectID=0;
		int   nObjectNum=0;
		
		std::string  szTitle="ËùĞè²ÄÁÏ:\n";
		char        szParse[256]={0};
		bool        bLine=true;

		if (aid.size()>0)
		{
			sprintf(szParse,"ËùĞè½ğÇ®: %d\n",aid[0]);
			szTitle=szParse+szTitle;
			if (aid[0]>GetScene()->GetMainCharacter()->GetItemCount(665))
			{
				bNeed=false;
			}
			for (int i=1;i<aid.size();i+=2)
			{
				nObjectID=aid[i];
				nObjectNum=aid[i+1];
				sprintf(szParse,"%s * %d %s",GetObjectBase(nObjectID)->strName,nObjectNum,bLine?"\n":"  ");
				szTitle=szTitle+szParse;
				//bLine=!bLine;

				if (nObjectNum>GetScene()->GetMainCharacter()->GetItemCount(nObjectID))
				{
					bNeed=false;
				}
			}
		}
		m_pLabel->SetText(szTitle.data());

	}
	return bNeed;
	

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pObject : ÃèÊö
 * \param pCell : ÃèÊö
 * \param bSelect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiItemMake::LoadMakeImage(ObjectBase_t* pObject,CGuiItemCell* pCell, bool bSelect)
{
	FUNCTION_BEGIN;

	stResourceLocation rl0 = GetObjectBasePicLocation(pObject);
	stResourceLocation rl1 = GetObjectBasePatternPicLocation(pObject);
	stResourceLocation rl2 = GetObjectBasePattern2PicLocation(pObject);
	pCell->Clear();


	if(!rl0.IsEmpty())
		pCell->AddAnimation(rl0);
	

	if(!rl1.IsEmpty())
		pCell->AddAnimation(rl1);
	if(!rl2.IsEmpty())
		pCell->AddAnimation(rl2);

	FUNCTION_END;
}
