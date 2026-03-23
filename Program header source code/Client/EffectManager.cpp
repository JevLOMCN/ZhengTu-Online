#include "public.h"

#include "../xml_parse/XmlParseEx.h"

#include "EffectManager.h"

CEffectManager::CEffectManager()
{
	this->LoadEffects();
}

CEffectManager::~CEffectManager()
{
	
}

bool CEffectManager::GetEffect(const std::string& strName, stEffectInfo& info)
{
	bool r = false;
	TEffectsMap::iterator iter = m_mapEffectInfos.find(strName);
	if (iter != m_mapEffectInfos.end())
	{
		info = iter->second;
		r = true;
	}

	return r;
}

void CEffectManager::ReloadEffects()
{
	m_mapEffectInfos.clear();
	this->LoadEffects();
}

void CEffectManager::LoadEffects()
{
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\effect.xml");
	if(!pStream)
	{
		Engine_WriteLog("×°ÔØ effect.xml Ê§°Ü\n");
		return ;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("½âÎö effect.xml Ê§°Ü\n");
		return;
	}

	TiXmlElement* pEmotion = doc.FirstChildElement("effect");
	if(pEmotion)
	{
		TiXmlElement* pIcon = pEmotion->FirstChildElement("eff");
		while(pIcon)
		{
			std::string strName;
			stEffectInfo info;
			const char* pName = pIcon->Attribute("Name");
			if( pName )
			{
				strName = std::string(pName);
			}

			const char* pFileName = pIcon->Attribute("FileName");
			if( pFileName )
			{
				strcpy(info.stResourceLocation.szFileName, pFileName);
			}

			if(pIcon->QueryIntAttribute("Group",&info.stResourceLocation.group) != TIXML_SUCCESS)
				continue;

			if(pIcon->QueryIntAttribute("Frame",&info.stResourceLocation.frame) != TIXML_SUCCESS)
				continue;

			if(pIcon->QueryIntAttribute("OffsetX",&info.nOffsetX) != TIXML_SUCCESS)
				continue;

			if(pIcon->QueryIntAttribute("OffsetY",&info.nOffsetY) != TIXML_SUCCESS)
				continue;

			m_mapEffectInfos.insert(std::make_pair(strName, info));

			pIcon = pIcon->NextSiblingElement("eff");
		}

	}
	ClosePackFileForRead(pStream);
}

CEffectManager& GetEffectManager()
{
	static CEffectManager effMgr;
	return effMgr;
}