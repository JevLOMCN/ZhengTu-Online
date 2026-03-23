/*
     Give the change for Player to give some nice or bad advices back 
     to us. GM can use these advices for better services for players.
*/
#include "public.h"
#include "./GameGuiManager.h"
#include "./GuiGmMsgBoard.h"
#include ".\gamestate.h"
#include "GameScene.h"
#include "MainCharacter.h"

// Is player must select one item from the drop-down list frist
#undef  _MANUAL_START_

// command id in *.xml 

// common id of all kinds of msg 
// drop-down list 
#define ID_COMBOX_MSGTYPE   1

// tab,which is triggered by the drop-down list  
#define ID_TAB              2 

// detail info 
#define ID_MLEDITBOX_DETAIL  4


// telephone 
#define ID_COMBOX_TLETYPE   5
#define ID_EDITBOX_TELNUM    6


// common button 
#define ID_BTN_CLOSE        7
#define ID_BTN_CANCEL       8  
#define ID_BTN_COMMIT       9
 
// separate id of special kind

// 1 : 外挂举报
#define  ID_1_EDIT_WHOWASREPORTED  101
#define  ID_1_EDIT_COUNTRY         102
#define  ID_1_EDIT_MAP             103
#define  ID_1_EDIT_POS             104
#define  ID_1_EDIT_YEAR            105
#define  ID_1_EDIT_MONTH           106
#define  ID_1_EDIT_DAY             107
#define  ID_1_EDIT_TIME            108
#define  ID_1_EDIT_PROGNAME        109

// 2  : bug提交
#define  ID_2_EDIT_COUNTRY         201 
#define  ID_2_EDIT_MAP             202 
#define  ID_2_EDIT_POS             203 
#define  ID_2_EDIT_YEAR            204 
#define  ID_2_EDIT_MONTH           205
#define  ID_2_EDIT_DAY             206
#define  ID_2_EDIT_TIME            207
 
// 3 : 游戏建议意见

// 4 : 不良游戏现象举报
#define  ID_3_COMBOX_BAD           401
#define  ID_3_EDIT_PROGNAME        402  
#define  ID_3_EDIT_COUNTRY         403 
#define  ID_3_EDIT_MAP             404
#define  ID_3_EDIT_POS             405 
#define  ID_3_EDIT_YEAR            406
#define  ID_3_EDIT_MONTH           407
#define  ID_3_EDIT_DAY             408
#define  ID_3_EDIT_TIME            409


// 5 : 游戏内容咨询




///////////////////////////////////////////////////////////////////////
const char c_Notify[]=
{
	"    亲爱的玩家,请详细描述你所反映的问题,以便\n"
	"GM能及时准确的为你排忧解难.GM的回复将发送到\n"
	"送到你游戏中的信箱里.\n"
	"    为了确保GM的工作质量,并能及时回复每一个\n"
	"玩家的咨询,在你提交问题后，半小时之内将不能\n"
	"再次使用GM信箱.感谢你的支持与关爱!\n"
	"        是否确认发送邮件?            "
};
/////////////////////////////////////////////////////////////////////

//struct stMsgGmTool : public stGmToolUserCmd
//{
//BYTE type;//定单类型
//char content[512];//主要内容
//BYTE contact;//联系方式
//char tele[64];//地址、电话
//char hisName[MAX_NAMESIZE];//被举报者
//char bugCountry[8];//bug发生国家
//char bugMap[16];//bug发生地图
//char bugPos[8];//bug发生位置
//char bugTime[64];//发生时间
//char progName[64];//疑似外挂名称
//BYTE behavior;//不良游戏现象
//};

//<item>外挂举报</item>
//<item>bug提交</item>
//<item>游戏建议意见</item>
//<item>不良游戏现象举报</item>
//<item>游戏内容咨询</item>
//---------------------------------------------------------------------
bool __CheckStringSafe(const char * pInner)
{
	FUNCTION_BEGIN;

	 

	size_t len = strlen(pInner); 
	char *pComment = (char*)_alloca(sizeof(char) * (len + 1));
	memcpy(pComment,pInner,len);
	pComment[len] = 0;


	str_trim_right(pComment);
	str_trim_left(pComment);

	len = strlen(pComment);
	return (len != 0);

	return false;

	FUNCTION_END;
}

bool GMSendMsgToServer(const char * pInner,int nMsgType,int nTelType,const char * pszTelephone)
{
	if(!GetGameState()->IsState(eGameState_Run)) 
		return false;

	if ( !__CheckStringSafe(pInner))	
		return false;

	stMsgGmTool  cmd;

	// 发送者姓名
	strncpy(cmd.hisName,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.hisName));
	cmd.hisName[sizeof(cmd.hisName)-1] = 0;


	// 发送内容
	SetFrameAllocator fa;
	size_t nNeedLen = strlen(pInner) + 1;
	char  * pTrim = (char *)FrameAllocator::alloc(nNeedLen);
	strncpy(pTrim,pInner,nNeedLen-1);
	pTrim[nNeedLen-1] = 0;
	str_trim_left(pTrim);
	str_trim_right(pTrim);
	strncpy(cmd.content,pTrim,sizeof(cmd.content));
	cmd.content[sizeof(cmd.content)-1] = 0;

	//
	cmd.type = 6;  // 6 代表客户端发现外挂
	cmd.contact = nTelType;

	if( pszTelephone)
	{
		strncpy(cmd.tele,pszTelephone,sizeof(cmd.tele));
		cmd.tele[sizeof(cmd.tele)-1] = 0;
	}
	SEND_USER_CMD(cmd);

	return true;
}

/////////////////////////////////////////////////////////////////////
CGuiGmMsgBoxDialog::CGuiGmMsgBoxDialog(void)
: m_guiDetailMEBox(NULL),
  m_pTab(NULL),
  m_guiTelNumEditBox(NULL),
  m_pMsgTypeComboBox(NULL),
  m_pTelTypeComboBox(NULL)
{
	FUNCTION_BEGIN;	

	m_rcMLineEdit[0].SetRect(35,221,550,330) ;    // 外挂举报
	m_rcMLineEdit[1].SetRect(35,221,550,330) ; // BUG提交
	m_rcMLineEdit[2].SetRect(35,102,550,330) ;//游戏建议提交
	m_rcMLineEdit[3].SetRect(35,221,550,330) ;//不良游戏现象举报
	m_rcMLineEdit[4].SetRect(35,102,550,330) ;//游戏内容咨询


	FUNCTION_END;
}

CGuiGmMsgBoxDialog::~CGuiGmMsgBoxDialog(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiGmMsgBoxDialog::InitCustomElements()
{
	FUNCTION_BEGIN;

	/*
	const char* szGrpFileName = GetGuiGraphicPackName();

	CGuiElement Element;
	stRectI rcTexture;
	stRectI rects[9];

	stResourceLocation rl;
	rl.SetFileName(GetGuiGraphicPackName());
	rl.group = 0;
	rl.frame = 0;

	//-------------------------------------
	// Element for the caption 标题框
	//-------------------------------------
	rcTexture.SetRect( 138, 60, 256, 86 );
	if(m_pCapElement == NULL)
	m_pCapElement = new CGuiElement;
	m_pCapElement->pBmp = NULL;
	m_pCapElement->TextureColor.States[ GUI_STATE_NORMAL ] = 0;
	m_pCapElement->FontColor.States[ GUI_STATE_NORMAL ] = 0;
	m_pCapElement->SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_CENTER | DT_VCENTER );
	m_pCapElement->TextureColor.Current = COLOR_ARGB(255,255,255,255);
	m_pCapElement->FontColor.Current = COLOR_ARGB(255,255,255,255);

	stRectI m_borderRect =  stRectI(0,0,256,256);
	//-------------------------------------
	// Element for the back 外框
	//-------------------------------------
	m_BackElements.resize( 9 );
	{
	rl.group = 11;
	rl.frame = 0;
	CuttRect9(m_borderRect,m_borderTopLeft,m_borderRightBottom,rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
	CGuiElement* pEmt = new CGuiElement;
	pEmt->SetTexture(&rl,&rects[i]);
	pEmt->FontColor.Current = COLOR_ARGB(255,255,255,255);
	pEmt->TextureColor.Current = COLOR_ARGB(255,255,255,255);
	m_BackElements[i] = pEmt;
	}
	}

	//------------------------------------
	// Element for the 3D Border EditBox(编辑框)
	//------------------------------------
	{
	rl.group = 11;
	rl.frame = 1;
	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_EDITBOX);
	pInfo->rcTexture.SetRect(31,0,111,25);
	pInfo->ptLeftTop = stPointI(5,5);
	pInfo->ptRightBottom = stPointI(5,5);
	pInfo->ptBorderLeftTop = stPointI(4,4);
	pInfo->ptBorderRightBottom = stPointI(4,4);
	pInfo->CuttRect9(rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
	pInfo->element[i].SetTexture(&rl,&rects[i]);
	pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	}

	//------------------------------------
	// Element for the 3D Border MLEditBox(多行编辑框)
	//------------------------------------
	{
	rl.group = 11;
	rl.frame = 1;
	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_MLEDITBOX);
	pInfo->rcTexture.SetRect(23,32,104,114);
	pInfo->ptLeftTop = stPointI(4,4);
	pInfo->ptRightBottom = stPointI(4,4);
	pInfo->ptBorderLeftTop = stPointI(4,4);
	pInfo->ptBorderRightBottom = stPointI(4,4);
	pInfo->CuttRect9(rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
	pInfo->element[i].SetTexture(&rl,&rects[i]);
	pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	}

	//------------------------------------
	// Element for the 3D Border ListBox(列表框)
	//------------------------------------
	{
	rl.group = 11;
	rl.frame = 1;
	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_LISTBOX);
	pInfo->rcTexture.SetRect(23,32,104,114);
	pInfo->ptLeftTop = stPointI(4,4);
	pInfo->ptRightBottom = stPointI(4,4);
	pInfo->ptBorderLeftTop = stPointI(4,4);
	pInfo->ptBorderRightBottom = stPointI(4,4);
	pInfo->CuttRect9(rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
	pInfo->element[i].SetTexture(&rl,&rects[i]);
	pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	}

	//------------------------------------
	// Element for the 3D Border ScrollBar(滚动条)
	//------------------------------------
	{
	rl.group = 11;
	rl.frame = 1;
	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
	pInfo->rcTexture.SetRect(2,13,20,118);
	pInfo->ptLeftTop = stPointI(3,22);
	pInfo->ptRightBottom = stPointI(3,22);
	pInfo->ptBorderLeftTop = stPointI(3,3);
	pInfo->ptBorderRightBottom = stPointI(3,3);
	pInfo->CuttRect9(rects);
	for(size_t i=0;i<count_of(rects);++i)
	{
	pInfo->element[i].SetTexture(&rl,&rects[i]);
	pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	}
	}


	//-------------------------------------
	// CGuiScrollBar - Up Arrow
	//-------------------------------------
	rcTexture.SetRect( 0, 0,10, 11);
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );

	//-------------------------------------
	// CGuiScrollBar - Down Arrow
	//-------------------------------------
	rcTexture.SetRect( 10,0 ,20, 11 );
	Element.SetTexture( &rl, &rcTexture );
	Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );

	//-------------------------------------
	// CGuiScrollBar - Button
	//-------------------------------------
	rcTexture.SetRect( 20, 0, 30, 28 );
	Element.SetTexture( &rl, &rcTexture );

	// Assign the Element
	SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	*/

	FUNCTION_END;
}

void CGuiGmMsgBoxDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_guiDetailMEBox = GetMLEditBox(ID_MLEDITBOX_DETAIL);
	m_guiTelNumEditBox = GetEditBox(ID_EDITBOX_TELNUM);

	m_pMsgTypeComboBox = GetComboBox(ID_COMBOX_MSGTYPE);
	m_pTelTypeComboBox = GetComboBox(ID_COMBOX_TLETYPE);

	
	m_pTab = GetTab(ID_TAB);
	if( !m_pTab )	return;
	m_pTab->SetVisible(false);
	

#ifdef _MANUAL_START_
	// All is sinless,woha! 
	// Now you must select a item first.
        //m_pMsgTypeComboBox->SetText("");

#else
         
#endif 
	if (m_pMsgTypeComboBox)
	{
		m_pMsgTypeComboBox->SetCurItem(1);
		SetTab(1);
		CGuiEditBox * pTemp = GetEditBox(ID_2_EDIT_COUNTRY);
		if (pTemp)
		{
			if (GetScene()->GetMapCountryName())
			{
				pTemp->SetText(GetScene()->GetMapCountryName());
			}			
		}
		pTemp = GetEditBox(ID_2_EDIT_MAP);
		if (pTemp)
		{
			if (GetScene()->GetMapName())
			{
				pTemp->SetText(GetScene()->GetMapName());
			}			
		}
		pTemp = GetEditBox(ID_2_EDIT_POS);
		if (pTemp)
		{
			char pos_str[24] = {"0,0"};
			sprintf(pos_str,"(%d,%d)",
				GetScene()->GetMainCharacter()->GetGridPos().x,
				GetScene()->GetMainCharacter()->GetGridPos().y);
			pTemp->SetText(pos_str);						
		}
	}	
	
	// Set the current dropdown height,just for seeing the whole items when the combox was 
	// dropped down 
	stGuiCtrlBorderInfo* pBorderInfo = GetCtrlBorderInfo(GUI_CONTROL_COMBOBOX);
	int iFontHeight = GetDevice()->GetFontHeight(m_pMsgTypeComboBox->GetFont());
	int iTrueDropHeight = getMax((int)m_pMsgTypeComboBox->GetItemCount(),(int)6) * iFontHeight + pBorderInfo->ptBorderLeftTop.y + pBorderInfo->ptBorderRightBottom.y;
	m_pMsgTypeComboBox->SetDropHeight(iTrueDropHeight+5);
	
	FUNCTION_END;
}

//-------------------------------------------------------------------------
// Select a tabItem 
//-------------------------------------------------------------------------
void CGuiGmMsgBoxDialog::SetTab(int nTab)
{
      FUNCTION_BEGIN;

      if( !m_pTab )  return;
      assert(nTab >= 0 && nTab < m_pTab->GetItemCount());
      if( nTab <0 || nTab >= m_pTab->GetItemCount()) 
	      return;

      if( nTab >= m_pTab->GetItemCount())
	      return;
      m_pTab->SetCurItem(nTab);


      if( m_guiDetailMEBox )
      {
	      assert( nTab >= 0 && nTab < count_of(m_rcMLineEdit) );
	      if( nTab <0 || nTab >= count_of(m_rcMLineEdit) ) 
		      return;

	      m_guiDetailMEBox->SetText("");
	      m_guiDetailMEBox->SetSize(m_rcMLineEdit[nTab].Width(),m_rcMLineEdit[nTab].Height());
	      m_guiDetailMEBox->SetLocation(m_rcMLineEdit[nTab].left,m_rcMLineEdit[nTab].top);

      }
      
      FUNCTION_END;
}

void CGuiGmMsgBoxDialog::OnClose(void)
{
	FUNCTION_BEGIN;
         
	GetGameGuiManager()->m_guiGmMsgBoxDialog = NULL;

	FUNCTION_END;
}

bool CGuiGmMsgBoxDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
			case ID_BTN_CLOSE:
			case ID_BTN_CANCEL:
				Close();
				break;
			case ID_BTN_COMMIT:
				if( CheckNotNull() )
				{
					GetGameGuiManager()->AddMessageBox(c_Notify,eAction_GMBox,NULL,c_szMBOKCancel);				
				}				
				break;
		}
		break;
	case EVENT_COMBOBOX_SELECTION_CHANGED:
		switch(nID)
		{
		case ID_COMBOX_MSGTYPE:
			{
				if(m_pMsgTypeComboBox )
				{
					int nCurItem =  m_pMsgTypeComboBox->GetCurItem();
					if ( nCurItem < 0 ) 
						break;
					if( nCurItem >= m_pMsgTypeComboBox->GetItemCount() )
						break;

					SetTab(nCurItem);
				}				
			}
			break;
		case ID_COMBOX_TLETYPE:
			{
				if( m_guiTelNumEditBox )
					SetFocus(m_guiTelNumEditBox);
			}
			break;
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


int  CGuiGmMsgBoxDialog::GetCurMsgType()
{

	 FUNCTION_BEGIN;

	 int nCurItem =  m_pMsgTypeComboBox->GetCurItem();
	 if ( nCurItem < 0 || nCurItem >= m_pMsgTypeComboBox->GetItemCount())
		 return -1;
	 return nCurItem;

	 FUNCTION_END;
}

int  CGuiGmMsgBoxDialog::GetCurTelType()
{

	FUNCTION_BEGIN;

	int nCurItem =  m_pTelTypeComboBox->GetCurItem();
	if ( nCurItem < 0 || nCurItem >= m_pTelTypeComboBox->GetItemCount())
		return -1;
	return nCurItem;

	FUNCTION_END;
}

//--------------------------------------------------------------------------------
//  Check the coment in the m_guiDetailMEBox must not be null
//--------------------------------------------------------------------------------
bool CGuiGmMsgBoxDialog::CheckNotNull()
{
	FUNCTION_BEGIN;

	const char * pDetail = m_guiDetailMEBox->GetText();

	if ( !__CheckStringSafe(pDetail))
	{
		GetGameGuiManager()->AddMessageBox("请填写详细内容!");
		return false;
	}
	return true;

	FUNCTION_END;
}

//-------------------------------------------------------------------------
// External function for send msg to server
//-------------------------------------------------------------------------
void CGuiGmMsgBoxDialog::SendMsg()
{
	if( !_SendMsgToServer())
	{
		GetGameGuiManager()->AddClientSystemMessage("对不起，你发送失败，请重新发送!");
	}
	Close();
}

//-------------------------------------------------------------------------
//  Internal function for send msg to server 
//-------------------------------------------------------------------------
bool CGuiGmMsgBoxDialog::_SendMsgToServer()
{
	FUNCTION_BEGIN;

	stMsgGmTool msg;

	// Common of all kinds of msg 
	int nCurMsgType = GetCurMsgType();
	if( -1 == nCurMsgType )
		return false;
	msg.type =  nCurMsgType;

	const char * pDetail = m_guiDetailMEBox->GetText();

	if ( !__CheckStringSafe(pDetail))
	{
		GetGameGuiManager()->AddMessageBox("请填写详细内容!");
		return false;
	}

	strncpy(msg.content,pDetail,sizeof(msg.content));

	int nCurTelType = GetCurTelType();
	if( -1 == nCurTelType )
		return false;

	msg.contact = nCurTelType;

	const char * pTelphone = m_guiTelNumEditBox->GetText();
	strncpy(msg.tele,pTelphone,sizeof(msg.tele));


	if( m_pTab->GetCurItem() == 0 )
	{ // 外挂举报 
		
		CGuiEditBox * pTemp = GetEditBox(ID_1_EDIT_WHOWASREPORTED);
		if( pTemp )
		{
			strncpy(msg.hisName,pTemp->GetText(),sizeof(msg.hisName));
		}

		pTemp = GetEditBox(ID_1_EDIT_COUNTRY);
		if( pTemp )
		{
			strncpy(msg.bugCountry,pTemp->GetText(),sizeof(msg.bugCountry));
		}

		pTemp = GetEditBox(ID_1_EDIT_MAP);
		if( pTemp )
		{
			strncpy(msg.bugMap,pTemp->GetText(),sizeof(msg.bugMap));
		}

		pTemp = GetEditBox(ID_1_EDIT_POS);
		if( pTemp )
		{
			strncpy(msg.bugPos,pTemp->GetText(),sizeof(msg.bugPos));
		}

		//size_t skip_time = sizeof(msg.bugTime) / ((size_t)4);

		pTemp  =  GetEditBox(ID_1_EDIT_YEAR);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText());
		}
		pTemp  =  GetEditBox(ID_1_EDIT_MONTH);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_1_EDIT_DAY);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_1_EDIT_TIME);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
                msg.bugTime[sizeof(msg.bugTime)-1] = 0;

		pTemp  =  GetEditBox(ID_1_EDIT_PROGNAME);
		if( pTemp )
		{
			strncpy(msg.progName,pTemp->GetText(),sizeof(msg.progName));
		}
	}
	else if (m_pTab->GetCurItem() == 1) 
	{  // bug提交
		CGuiEditBox * pTemp = GetEditBox(ID_2_EDIT_COUNTRY);
		if( pTemp )
		{
			strncpy(msg.bugCountry,pTemp->GetText(),sizeof(msg.bugCountry));
		}
		pTemp = GetEditBox(ID_2_EDIT_MAP);
		if( pTemp )
		{
			strncpy(msg.bugMap,pTemp->GetText(),sizeof(msg.bugMap));
		}

		pTemp = GetEditBox(ID_2_EDIT_POS);
		if( pTemp )
		{
			strncpy(msg.bugPos,pTemp->GetText(),sizeof(msg.bugPos));
		}

		pTemp  =  GetEditBox(ID_2_EDIT_YEAR);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_2_EDIT_MONTH);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_2_EDIT_DAY);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_2_EDIT_TIME);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		 msg.bugTime[sizeof(msg.bugTime)-1] = 0;
	}
	else if (m_pTab->GetCurItem() == 2)
	{ // 游戏建议意见

	}
	else if (m_pTab->GetCurItem() == 3)
	{ // 不良游戏现象举报
	 	CGuiComboBox * pTempComboBox = GetComboBox(ID_3_COMBOX_BAD);
		if( pTempComboBox )
		{
			msg.behavior = pTempComboBox->GetCurItem();
		}

		CGuiEditBox * pTemp = GetEditBox(ID_3_EDIT_PROGNAME);
		if( pTemp )
		{
			strncpy(msg.progName,pTemp->GetText(),sizeof(msg.progName));
		}

		pTemp = GetEditBox(ID_3_EDIT_COUNTRY);
		if( pTemp )
		{
			strncpy(msg.bugCountry,pTemp->GetText(),sizeof(msg.bugCountry));
		}

		pTemp = GetEditBox(ID_3_EDIT_MAP);
		if( pTemp )
		{
			strncpy(msg.bugMap,pTemp->GetText(),sizeof(msg.bugMap));
		}

		pTemp = GetEditBox(ID_3_EDIT_POS);
		if( pTemp )
		{
			strncpy(msg.bugPos,pTemp->GetText(),sizeof(msg.bugPos));
		}

		size_t skip_time = sizeof(msg.bugTime) / ((size_t)4);

		pTemp  =  GetEditBox(ID_3_EDIT_YEAR);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_3_EDIT_MONTH);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_3_EDIT_DAY);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}
		pTemp  =  GetEditBox(ID_3_EDIT_TIME);
		if( pTemp )
		{
			strcat(msg.bugTime ,pTemp->GetText() );
		}	
		msg.bugTime[sizeof(msg.bugTime)-1] = 0;
	}
	else if( m_pTab->GetCurItem() == 4 )
	{// 游戏内容咨询

	}

	SEND_USER_CMD(msg);

	return true;

	FUNCTION_END;
}

// this is a fuck (very very bad function)
void CGuiGmMsgBoxDialog::LimitDetaiInfo()
{
#if 0 
#define ID_MAX_CHAR_COUNT  20
	if ( m_guiDetailMEBox )
	{
		//if ( m_guiDetailMEBox->GetStyle() & CGuiMLEditBox::GUI_ES_MULTIMEDIA )  return;
		const char* szText = m_guiDetailMEBox->GetText();
		size_t len = strlen(szText);
		if( len >= ID_MAX_CHAR_COUNT)
		{
			char szTemp[ID_MAX_CHAR_COUNT] = {0};
			memcpy(szTemp,szText,ID_MAX_CHAR_COUNT-1);
			

			size_t i;
			for(i=0;i < ID_MAX_CHAR_COUNT;)
			{
				if(szTemp[i] & 0x80)
				{
					if( i + 1 >= ID_MAX_CHAR_COUNT)
						break;
					i+=2;
				}
				else
				{
					i++;
				}
			}
			szTemp[ID_MAX_CHAR_COUNT-1] = 0;
			m_guiDetailMEBox->SetText(szTemp);
		}
	}
#endif 
}

//-------------------------------------------------------------------------------------
// When the combox was dropped down,the range of all the opened items will
// extend the dialog.So when you click the item which layed out of the dialog,
// the click msg will be invalid.By extending the combox with the PtInCombox() can solve 
// the problem.
//-------------------------------------------------------------------------------------
DWORD CGuiGmMsgBoxDialog::OnHiTest(POINT ptScreen)
{
	FUNCTION_BEGIN;

	if( IsVisible())
	{
		if(m_pTelTypeComboBox && m_pTelTypeComboBox->PtInCombox(ptScreen))
		{
			return TRUE;
		}
	}

	return CGuiDialog::OnHiTest(ptScreen);

	FUNCTION_END;
}
