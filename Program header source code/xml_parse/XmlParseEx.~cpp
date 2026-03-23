#ifdef __BORLANDC__
#else
#include "../engine/include/engine.h"
#endif
#include "XmlParseEx.h"

TiXmlComment* XML_FindAndAddComment(TiXmlNode* parent)
{
	TiXmlNode* child = parent->FirstChild();
	TiXmlComment* pComment = NULL;
	while(child)
	{
		if(pComment = child->ToComment())
			return pComment;
		child = child->NextSibling();
	}
	return parent->InsertEndChild(TiXmlComment())->ToComment();
}

TiXmlElement* XML_GetMatchingChildNode(TiXmlNode* parent,const char* elementName,const char* fieldName,const char* fieldValue)
{
	TiXmlElement* child = parent->FirstChildElement(elementName);
	while(child)
	{
		const char* value = child->Attribute(fieldName);
		if(value && strcmp(value,fieldValue) == 0)
			return child;
		child = child->NextSiblingElement(elementName);
	}
	return NULL;
}

TiXmlElement* XML_FindAndAddMatchingChildNode(TiXmlNode * parent,const char* elementName,const char* fieldName,const char* fieldValue)
{
	TiXmlElement* node = XML_GetMatchingChildNode(parent,elementName,fieldName,fieldValue);
	if(node) return node;
	node = parent->InsertEndChild(TiXmlElement(elementName))->ToElement();
	if(node)
	{
		node->SetAttribute(fieldName,fieldValue);
	}
	return node;
}

TiXmlElement* XML_GetMatchingChildNode(TiXmlNode* parent,const char* elementName,const char* fieldName,int fieldValue)
{
	TiXmlElement* child = parent->FirstChildElement(elementName);
	while(child)
	{
		int v;
		if(child->QueryIntAttribute(fieldName,&v) == TIXML_SUCCESS)
		{
			if(v == fieldValue) return child;
		}

		child = child->NextSiblingElement(elementName);
	}
	return NULL;
}

TiXmlElement* XML_FindAndAddMatchingChildNode(TiXmlNode * parent,const char* elementName,const char* fieldName,int fieldValue)
{
	TiXmlElement* node = XML_GetMatchingChildNode(parent,elementName,fieldName,fieldValue);
	if(node) return node;
	node = parent->InsertEndChild(TiXmlElement(elementName))->ToElement();
	if(node)
	{
		node->SetAttribute(fieldName,fieldValue);
	}
	return node;
}

TiXmlElement* XML_FindAndAddChildNode(TiXmlNode* parent,const char* elementName)
{
	TiXmlElement* node = parent->FirstChildElement(elementName);
	if(node) return node;
	node = parent->InsertEndChild(TiXmlElement(elementName))->ToElement();
	return node;
}

TiXmlText* XML_GetNodeTextNode(TiXmlElement* parent)
{
	TiXmlNode* child = parent->FirstChild();
	while(child)
	{
		if(child->ToText()) 
			return child->ToText();
		child  = child->NextSibling();
	}
	return NULL;
}

void XML_SetNodeText(TiXmlElement* parent,const char* pszText)
{
	TiXmlText* pText = XML_GetNodeTextNode(parent);
	if(!pText)
	{
		if(pszText)
			pText = parent->InsertEndChild(TiXmlText(pszText))->ToText();
	}
	else 
	{
		if(pszText)
			pText->SetValue(pszText);
		else
			parent->RemoveChild(pText);
	}
}

const char* XML_GetNodeText(TiXmlElement* parent)
{
	static char szNull[] = "";
	TiXmlText* pText = XML_GetNodeTextNode(parent);
	if(!pText) return szNull;
	return pText->Value();
}

TiXmlElement * XML_GetMatchingTextChildNode(TiXmlNode* parent,const char* elementName,const char* pszText)
{
	TiXmlElement* child = parent->FirstChildElement(elementName);
	while(child)
	{
		if(strcmp(XML_GetNodeText(child),pszText) == 0)
			return child;
		child = child->NextSiblingElement(elementName);
	}
	return NULL;
}

TiXmlElement * XML_FindAndAddMatchingTextChildNode(TiXmlNode* parent,const char* elementName,const char* pszText)
{
	TiXmlElement* child = XML_GetMatchingTextChildNode(parent,elementName,pszText);
	if(child) return child;
	child = parent->InsertEndChild(TiXmlElement(elementName))->ToElement();
	if(child)
	{
		XML_SetNodeText(child,pszText);
	}
	return child;
}
#ifndef __BORLANDC__
bool LoadXMLFromStream(Stream* i_stream,TiXmlDocument& doc)
{
	size_t size = i_stream->getSize();
	SetFrameAllocator fa;
	char* data = (char*)FrameAllocator::alloc(size + 1);
	i_stream->read(data,size);
	data[size]  =  0;
	return doc.Parse(data) != NULL;
}
#endif