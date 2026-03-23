#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../xml_parse/XmlParseEx.h"


static char szUnknown[] = "Unknown";
struct stControlTypeDesc{
	int iControlType;
	char szDesc[16];
};
static stControlTypeDesc guiControlTypeDesc[] = {
	{GUI_CONTROL_BUTTON,"Button"},
	{GUI_CONTROL_STATIC,"Static"},
	{GUI_CONTROL_CHECKBOX,"CheckBox"},
	{GUI_CONTROL_RADIOBUTTON,"RadioButton"},
	{GUI_CONTROL_COMBOBOX,"ComboBox"},
	{GUI_CONTROL_SLIDER,"Slider"},
	{GUI_CONTROL_EDITBOX,"EditBox"},
	{GUI_CONTROL_MLEDITBOX,"MLineEditBox"},
	{GUI_CONTROL_LISTBOX,"ListBox"},
	{GUI_CONTROL_SCROLLBAR,"ScrollBar"},
	{GUI_CONTROL_IMAGE,"Image"},
	{GUI_CONTROL_ITEMCELL,"ItemCell"},
	{GUI_CONTROL_TABLE,"Table"},
	{GUI_CONTROL_TAB,"Tab"},
	{GUI_CONTROL_TAB,"Panel"},
	{GUI_CONTROL_PROCESS,"Process"},
	{GUI_CONTROL_SCROLLTAB,"ScrollTab"},
	{GUI_CONTROL_MLTEXTBOX,"MLineTextBox"},
	{GUI_CONTROL_CLOSEBOX,"CloseBox"},
	{GUI_CONTROL_TREE,"Tree"},
};

static char szControlState[][16] = {
	"NORMAL",
	"DISABLED",
	"HIDDEN",
	"FOCUS",
	"MOUSEOVER",
	"PRESSED",
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iControlType : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const char* GetGuiControlTypeDesc(int iControlType)
{
	for(size_t i=0;i<count_of(guiControlTypeDesc);++i)
	{
		if(guiControlTypeDesc[i].iControlType == iControlType)
			return guiControlTypeDesc[i].szDesc;
	}
	
	return szUnknown;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param desc : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int DescToGuiControlType(const char* desc)
{
	char* p;
	long l = strtol(desc,&p,10);
	if(p != desc)
		return l;

	for(size_t i=0;i<count_of(guiControlTypeDesc);++i)
	{
		if(stricmp(desc,guiControlTypeDesc[i].szDesc) == 0)
			return guiControlTypeDesc[i].iControlType;
	}
	return 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iState : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const char* GetGuiControlStateDesc(int iState)
{
	if((size_t)iState < count_of(szControlState))
		return szControlState[iState];
	return szUnknown;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param desc : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int DescToGuiControlState(const char* desc)
{
	char* p;
	long l = strtol(desc,&p,10);
	if(p != desc)
		return l;

	for(size_t i=0;i<count_of(szControlState);++i)
	{
		if(stricmp(desc,szControlState[i]) == 0)
			return i;
	}
	return 0;
}





///**
// * \brief ºÚ∂Ã√Ë ˆ
// * 
// * œÍœ∏√Ë ˆ
// * 
// * \param strXML : √Ë ˆ
// * \param strResult : √Ë ˆ
// * \param level : √Ë ˆ
// * \param data : √Ë ˆ
// * \param defaultColor : √Ë ˆ
// * \return ∑µªÿ÷µµƒ√Ë ˆ
// */
//bool GetTextFromXML(const char* strXML,std::string& strResult,int &level,std::vector<DWORD> *data,DWORD defaultColor)
//{
//	level = 0;
//	strResult = "";
//	if(data) data->resize(0);
//
//	TiXmlDocument doc;
//	doc.Parse(strXML);
//	if(doc.Error()) return false;
//	TiXmlElement * body = doc.FirstChildElement("body");
//	if(!body) return false;
//
//	TiXmlElement* p = body->FirstChildElement("p");
//	while(p)
//	{
//		TiXmlElement* n = p->FirstChildElement("n");
//		while(n)
//		{
//			DWORD color;
//			const char* var;
//			int task;
//			int face;
//			color = XML_GetColorAttribute(n,"color",defaultColor);
//			var = n->Attribute("var");
//
//			if(n->QueryIntAttribute("task",&task) != TIXML_SUCCESS)
//				task = -1;
//			
//			if(n->QueryIntAttribute("face",&face) != TIXML_SUCCESS)
//				face = -1;
//			if(face != -1)
//			{
//				WORD szFace[2];
//				szFace[0] = c_nPicCharBase + face;
//				szFace[1] = 0;
//				strResult += (char*)szFace;
//				if(data) data->push_back(xtimeGetTime());
//			}
//			else
//			{
//				std::string str;
//				if(task != -1 && var)
//				{
//					if(fnGetTaskVariable)
//					{
//						str = fnGetTaskVariable(task,var);
//					}
//				}
//				else 
//				{
//					str = XML_GetNodeText(n);
//				}
//
//				int count = GetCharacterCount(str.c_str());
//				if(data)
//				{
//					for(int i=0;i<count;++i)
//					{
//						data->push_back(color);
//					}
//				}
//				strResult += str;
//			}
//			n = n->NextSiblingElement("n");
//		}
//		
//		strResult += '\n';
//		if(data) data->push_back(0);
//
//		p = p->NextSiblingElement("p");
//	}
//
//	TiXmlElement* diffculty = body->FirstChildElement("diffcult");
//	if(diffculty)
//	{
//		if(diffculty->QueryIntAttribute("level",&level) != TIXML_SUCCESS)
//			level = 0;
//	}
//	return true;
//}