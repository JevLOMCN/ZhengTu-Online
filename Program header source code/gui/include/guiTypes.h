#ifndef _GUI_TYPES_H
#define _GUI_TYPES_H

//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
//#define MAX_CONTROL_STATES     16

// ---------------------------------------------
// 消息定义
// ---------------------------------------------
#define EVENT_CONTROL_KEYDOWN				1
#define EVENT_CONTROL_MOUSEENTER			2
#define EVENT_CONTROL_MOUSELEAVE			3

#define EVENT_BUTTON_CLICKED                100
#define EVENT_RBUTTON_CLICKED               101
#define EVENT_SHELLOBJECT_CLICKED			102
#define EVENT_LBUTTON_DOWN					103
#define EVENT_ITEM_CLICKED                  104


#define EVENT_SCRIPT_BUTTON_CLICK			105
#define EVENT_BUTTON_DRAG_BEGIN				106
#define EVENT_BUTTON_DRAG_END				107
#define EVENT_BUTTON_DRAG_MOVE				108
#define EVENT_HYPERLINK_CLICKED				109

#define EVENT_ITEM_RIGHT_CLICKED            110

#define EVENT_COMBOBOX_SELECTION_CHANGED    200

#define EVENT_RADIOBUTTON_CHANGED           300

#define EVENT_CHECKBOX_CHANGED              400

#define EVENT_SLIDER_VALUE_CHANGED          500

#define EVENT_EDITBOX_STRING                600
#define EVENT_EDITBOX_CHANGE                700

#define EVENT_LISTBOX_ITEM_DBLCLK           800
#define EVENT_LISTBOX_ITEM_CLICK           850
#define EVENT_LISTBOX_SELECTION             900
#define EVENT_LISTBOX_CHANGED              950
#define EVENT_LISTBOX_MOVESELECTION         951
			
#define EVENT_DIALOG_RESIZE					1000
#define EVENT_DIALOG_CREATE					1001
#define EVENT_DIALOG_SHOW					1002
#define EVENT_DIALOG_HIDE					1003
#define EVENT_DIALOG_MOVE					1004
#define EVENT_DIALOG_COMMAND				1005
#define EVENT_TAB_SELECTION_CHANGED			2000

//新定义宝盒转动消息
#define EVENT_BOX_BUTTONID2			        2001
#define EVENT_BOX_ANIMATIONEND              2002
#define EVENT_BOX_ANIMATIONSTAT             2003
//-----------------------------------------------
//-----------------------------------------------

#define ID_EDIT_CLEAR                   0xE120
#define ID_EDIT_COPY                    0xE122
#define ID_EDIT_CUT                     0xE123
#define ID_EDIT_PASTE                   0xE125
#define ID_EDIT_SELECT_ALL              0xE12A

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B // (not always defined)
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C // (not always defined)
#endif
#ifndef WM_XBUTTONDBLCLK
#define WM_XBUTTONDBLCLK                0x020D
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 8

// DXUT_MAX_EDITBOXLENGTH is the maximum string length allowed in edit boxes,
// including the NULL terminator.
// 
// Uniscribe does not support strings having bigger-than-16-bits length.
// This means that the string must be less than 65536 characters long,
// including the NULL terminator.
#define GUI_MAX_EDITBOXLENGTH 0xFFFF

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CGuiControl;
class CGuiButton;
class CGuiStatic;
class CGuiCheckBox;
class CGuiRadioButton;
class CGuiComboBox;
class CGuiSlider;
class CGuiEditBox;
class CGuiImeEditBox;
class CGuiListBox;
class CGuiScrollBar;
class CGuiElement;
class CGuiImage;
class CGuiDialog;
class CGuiItemCell;
class CGuiTable;
struct GuiElementHolder;
class CGuiTab;
class CGuiProcess;
class CGuiScrollTab;
class CGuiMLTextBox;
class CGuiTree;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
//typedef VOID (CALLBACK *PCALLBACKGUIEVENT) ( UINT nEvent, int nControlID, CGuiControl* pControl );


#define GUI_CONTROL_TOP        ((CGuiControl*)0)
#define GUI_CONTROL_BOTTOM     ((CGuiControl*)1)
//#define GUI_CONTROL_TOPMOST    ((CGuiControl*)-1)
//#define GUI_CONTROL_NOTOPMOST  ((CGuiControl*)-2)

#define GUI_DIALOG_TOP        ((CGuiDialog*)0)
#define GUI_DIALOG_BOTTOM     ((CGuiDialog*)1)
#define GUI_DIALOG_TOPMOST    ((CGuiDialog*)-1)
#define GUI_DIALOG_NOMOST  ((CGuiDialog*)-2)
#define GUI_DIALOG_BOTTOMMOST ((CGuiDialog*)2)

//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum GUI_CONTROL_TYPE 
{ 
	GUI_CONTROL_BUTTON		=	0, 
	GUI_CONTROL_STATIC		=	1, 
	GUI_CONTROL_CHECKBOX	=	2,
	GUI_CONTROL_RADIOBUTTON	=	3,
	GUI_CONTROL_COMBOBOX	=	4,
	GUI_CONTROL_SLIDER		=	5,
	GUI_CONTROL_EDITBOX		=	6,
	GUI_CONTROL_MLEDITBOX	=	7,
	GUI_CONTROL_LISTBOX		=	8,
	GUI_CONTROL_SCROLLBAR	=	9,
	GUI_CONTROL_IMAGE		=	10,
	GUI_CONTROL_ITEMCELL	=	11,
	GUI_CONTROL_TABLE		=	12,
	GUI_CONTROL_TAB			=	13,
	GUI_CONTROL_PANEL		=	14,
	GUI_CONTROL_PROCESS		=	15,
	GUI_CONTROL_SCROLLTAB	=	16,
	GUI_CONTROL_MLTEXTBOX	=	17,
	GUI_CONTROL_TREE        =   18,
	GUI_CONTROL_NUMBER,
	// 特殊控件
	GUI_CONTROL_CLOSEBOX	=	-1,
};

enum GUI_CONTROL_STATE
{
	GUI_STATE_NORMAL,
	GUI_STATE_DISABLED,
	GUI_STATE_HIDDEN,
	GUI_STATE_FOCUS,
	GUI_STATE_MOUSEOVER,
	GUI_STATE_PRESSED,
	MAX_CONTROL_STATES,
};

enum GUI_ALIGN_TYPE
{
	GUI_ALIGN_NONE = 0,
	GUI_ALIGN_LEFT = 0x1,
	GUI_ALIGN_HCENTER =0x2,
	GUI_ALIGN_RIGHT = 0x4,
	GUI_ALIGN_TOP	= 0x8,
	GUI_ALIGN_VCENTER = 0x10,
	GUI_ALIGN_BOTTOM = 0x20,
};

enum GUI_TOOLTIPS_TYPE
{
	GUI_ALIGN_LEFTTOP = 1,
	GUI_ALIGN_LEFTBUTTOM = 2,
};
extern const char* GetGuiControlTypeDesc(int iControlType);
extern const char* GetGuiControlStateDesc(int iState);
extern int DescToGuiControlType(const char* desc);
extern int DescToGuiControlState(const char* desc);

extern bool	 g_bWaitNextChar;
extern char	 g_szCurInput[3];
extern bool  g_bHideCaret;
//------------------------------------------------------
// Dialog Style:
//------------------------------------------------------
#define GUI_WS_TOPMOST		0x1
#define GUI_WS_BOTTOMMOST	0x2
#define GUI_WS_NOMOVE		0x4
#define GUI_WS_RESIZEING	0x8
#define GUI_WS_CLOSEBOX		0x10
#define GUI_WS_MODAL		0x20
#define GUI_WS_NOSKIN		0x40
#define GUI_WS_AUTOANCHOR	0x80 // 自动停靠
#define GUI_WS_SYSTEM		0x100
//------------------------------------------------------

#define GUI_CLOSEBOX_ID		-1

//extern bool GetTextFromXML(const char* strXML,std::string& strResult,int &level,std::vector<DWORD> *data = NULL,DWORD defaultColor = -1);
//extern std::string (*fnGetTaskVariable)(DWORD dwTaskID,const char* variableName);

#include "./GuiElement.h"
#include "./guiManager.h"
#include "./guiControl.h"
#include "./guiStatic.h"
#include "./guiButton.h"
#include "./guiComboBox.h"
#include "./guiEditBox.h"
#include "./guiRadioButton.h"
#include "./guiCheckBox.h"
#include "./guiListBox.h"
#include "./guiSlider.h"
#include "./guiScrollBar.h"
#include "./guiItemCell.h"
#include "./guiTable.h"
#include "./gui_script.h"
#include "./Cursor.h"
#include "./GuiTab.h"
#include "./GuiPanel.h"
#include "./GuiProcess.h"
#include "./GuiScrollTab.h"
#include "./GuiMenu.h"
#include "./GuiMLTextBox.h"
#include "./GuiTree.h"
#include "./guidialog.h"
#endif