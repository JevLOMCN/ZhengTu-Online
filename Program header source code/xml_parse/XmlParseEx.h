#ifndef _XMLPARSEEX_H
#define _XMLPARSEEX_H

#include "TinyXml.h"
#include <stdlib.h>

#ifndef __BORLANDC__
class Stream;
bool LoadXMLFromStream(Stream* i_stream,TiXmlDocument& doc);
#endif

TiXmlComment* XML_FindAndAddComment(TiXmlNode* parent);
TiXmlElement* XML_GetMatchingChildNode(TiXmlNode* parent,const char* elementName,const char* fieldName,const char* fieldValue);
TiXmlElement* XML_FindAndAddMatchingChildNode(TiXmlNode * parent,const char* elementName,const char* fieldName,const char* fieldValue);
TiXmlElement* XML_GetMatchingChildNode(TiXmlNode* parent,const char* elementName,const char* fieldName,int fieldValue);
TiXmlElement* XML_FindAndAddMatchingChildNode(TiXmlNode * parent,const char* elementName,const char* fieldName,int fieldValue);

TiXmlElement* XML_FindAndAddChildNode(TiXmlNode* parent,const char* elementName);

void XML_SetNodeText(TiXmlElement* parent,const char* pszText);
const char* XML_GetNodeText(TiXmlElement* parent);
TiXmlText* XML_GetNodeTextNode(TiXmlElement* parent);

TiXmlElement * XML_GetMatchingTextChildNode(TiXmlNode* parent,const char* elementName,const char* pszText);
TiXmlElement * XML_FindAndAddMatchingTextChildNode(TiXmlNode* parent,const char* elementName,const char* pszText);

#ifndef __BORLANDC__
inline DWORD XML_GetColorAttribute(TiXmlElement* node,const char* strAttrName,DWORD dwDefault = 0)
{
	const char *szValue = node->Attribute(strAttrName);
	if(szValue)
		return StringToColor(szValue);
	return dwDefault;
}

inline RECT XML_GetRectAttribute(TiXmlElement* node,const char* strAttrName)
{
	const char *szValue = node->Attribute(strAttrName);
	stRectI rc(0,0,0,0);
	if(szValue)
		rc.FromString(szValue);
	return rc;
}

inline void XML_SetColorAttribute(TiXmlElement* node,const char* strAttrName,DWORD color)
{
	char szValue[32];
	ColorToString(color,szValue,sizeof(szValue));
	node->SetAttribute(strAttrName,szValue);
}

inline void XML_SetRectAttribute(TiXmlElement* node,const char* strAttrName,const RECT* prc)
{
	stRectI rc = *prc;
	char szValue[32];
	rc.ToString(szValue,sizeof(szValue));
	node->SetAttribute(strAttrName,szValue);
}
#endif

inline void XML_SetAttribute(TiXmlElement* node,const char* strAttrName,int value,int radix)
{
	char szValue[32];
	if(itoa(value,szValue,radix))
	{
		node->SetAttribute(strAttrName,szValue);
	}
}

inline int XML_QueryAttribute(TiXmlElement* node,const char* strAttrName,int radix,int * value)
{
	const char* szValue = node->Attribute(strAttrName);
	if(szValue)
	{
		char* p;
		*value = strtoul(szValue,&p,radix);
		if(p != szValue)
			return TIXML_SUCCESS;
		return TIXML_WRONG_TYPE;
	}
	return TIXML_NO_ATTRIBUTE;
}

#endif