#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* c_szNil = "";
// -------------------------------------------
// DialogBox ÊôĞÔ
// -------------------------------------------
int GuiGetDialogX(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetX();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetDialogY(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetY();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param cx : ÃèÊö
 * \param cy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetDialogSize(tGuiDialogHandle dlgHdl,int cx,int cy)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return;
	pDlg->SetSize(cx,cy);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetDialogLocation(tGuiDialogHandle dlgHdl,int x,int y)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return;
	pDlg->SetLocation(x,y);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetDialogWidth(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetWidth();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetDialogHeight(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetHeight();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetDialogClientWidth(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetClientWidth();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetDialogClientHeight(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return pDlg->GetClientHeight();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param bEnable : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetDialogEnabled(tGuiDialogHandle dlgHdl,bool bEnable)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return ;
	pDlg->SetEnabled(bEnable);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GuiDialogIsEnabled(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return false;
	return pDlg->IsEnabled();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param bVisible : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetDialogVisible(tGuiDialogHandle dlgHdl,bool bVisible)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return ;
	pDlg->SetVisible(bVisible);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GuiDialogIsVisible(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return false;
	return pDlg->IsVisible();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiCloseDialog(tGuiDialogHandle dlgHdl)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return;
	pDlg->Close();
}
//------------------------------------------------------
//------------------------------------------------------


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
tGuiControlHandle GuiGetControl(tGuiDialogHandle dlgHdl,int id)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return static_cast<tGuiControlHandle>(pDlg->GetControl(id));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
tGuiControlHandle GuiGetEditBox(tGuiDialogHandle dlgHdl,int id)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return static_cast<tGuiControlHandle>(pDlg->GetEditBox(id));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
tGuiControlHandle GuiGetMLEditBox(tGuiDialogHandle dlgHdl,int id)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return static_cast<tGuiControlHandle>(pDlg->GetMLEditBox(id));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
tGuiControlHandle GuiGetButton(tGuiDialogHandle dlgHdl,int id)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return static_cast<tGuiControlHandle>(pDlg->GetButton(id));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlgHdl : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
tGuiControlHandle GuiGetCheckBox(tGuiDialogHandle dlgHdl,int id)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlgHdl);
	if(!pDlg) return 0;
	return static_cast<tGuiControlHandle>(pDlg->GetCheckBox(id));
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param edtHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------
// ¿Ø¼şÊôĞÔ
//-------------------------------------------------------
int GuiGetControlX(tGuiControlHandle edtHdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(edtHdl);
	if(!pControl) return NULL;
	return pControl->GetX();
}
int GuiGetControlY(tGuiControlHandle edtHdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(edtHdl);
	if(!pControl) return NULL;
	return pControl->GetY();
}
int GuiGetControlWidth(tGuiControlHandle edtHdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(edtHdl);
	if(!pControl) return NULL;
	return pControl->GetWidth();
}
int GuiGetControlHeight(tGuiControlHandle edtHdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(edtHdl);
	if(!pControl) return 0;
	return pControl->GetHeight();
}
void GuiSetControlLocation(tGuiControlHandle hdl,int x,int y)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return;
	pControl->SetLocation(x,y);
}
void GuiSetControlSize(tGuiControlHandle hdl,int cx,int cy)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return;
	pControl->SetSize(cx,cy);
}
void GuiSetControlText(tGuiControlHandle hdl,const char* text)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return;
	pControl->SetText(text);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param edtHdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* GuiGetControlText(tGuiControlHandle edtHdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(edtHdl);
	if(!pControl) return c_szNil;
	return pControl->GetText();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param tab : ÃèÊö
 * \param index : ÃèÊö
 * \param control : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiTabAddControl(tGuiControlHandle tab,int index,tGuiControlHandle control)
{
	CGuiTab* pTab = static_cast<CGuiTab*>(tab);
	if(!pTab) return ;
	pTab->AddControl(index,static_cast<CGuiControl*>(control));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param szText : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiAddControlItem(tGuiControlHandle hdl,const char* szText)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return;
	pControl->AddItem(szText,NULL);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param bChecked : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetChecked(tGuiControlHandle hdl,bool bChecked)
{
	CGuiCheckBox* pCheckBox = static_cast<CGuiCheckBox*>(hdl);
	if(!pCheckBox) return;
	pCheckBox->SetChecked(bChecked);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GuiGetChecked(tGuiControlHandle hdl)
{
	CGuiCheckBox* pCheckBox = static_cast<CGuiCheckBox*>(hdl);
	if(!pCheckBox) return false;
	return pCheckBox->GetChecked();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dlg : ÃèÊö
 * \param nIDFirstButton : ÃèÊö
 * \param nIDLastButton : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetCheckedRadioButton(tGuiDialogHandle dlg,int nIDFirstButton,int nIDLastButton)
{
	CGuiDialog* pDlg = static_cast<CGuiDialog*>(dlg);
	if(!pDlg) return 0;
	return pDlg->GetCheckedRadioButton(nIDFirstButton,nIDLastButton);
}
int  GuiGetControlCurItem(tGuiControlHandle hdl)
{ 
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return -1;
	return pControl->GetCurItem();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int  GuiGetControlItemCount(tGuiControlHandle hdl)
{ 
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return 0;
	return pControl->GetItemCount();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlCurItem(tGuiControlHandle hdl,int index)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetCurItem(index);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param nValue : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlValue(tGuiControlHandle hdl, int nValue )
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetValue(nValue);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int  GuiGetControlValue(tGuiControlHandle hdl) 
{ 
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return 0;
	return pControl->GetValue();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param nMin : ÃèÊö
 * \param nMax : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlRange( tGuiControlHandle hdl,int nMin, int nMax )
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetRange(nMin,nMax);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetControlRangeMin(tGuiControlHandle hdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return 0;
	return pControl->GetRangeMin();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetControlRangeMax(tGuiControlHandle hdl)
{ 
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return 0;
	return pControl->GetRangeMax();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int GuiGetControlPageSize(tGuiControlHandle hdl) 
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return 0;
	return pControl->GetPageSize(); 
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param nPageSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlPageSize(tGuiControlHandle hdl, int nPageSize )
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetPageSize(nPageSize);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlVisible(tGuiControlHandle hdl,bool b)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetVisible(b);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \param b : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiSetControlEnabled(tGuiControlHandle hdl,bool b)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return ;
	pControl->SetEnabled(b);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GuiGetControlVisible(tGuiControlHandle hdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return false;
	return pControl->IsVisible();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param hdl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GuiGetControlEnabled(tGuiControlHandle hdl)
{
	CGuiControl* pControl = static_cast<CGuiControl*>(hdl);
	if(!pControl) return false;
	return pControl->IsEnabled();
}
//*/
//--------------------------------------------------------
//--------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param lua : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiScriptInit(lua_State * lua)
{
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiScriptUninit()
{
}