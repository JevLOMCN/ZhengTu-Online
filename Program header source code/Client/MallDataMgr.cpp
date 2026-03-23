#include "public.h"

#include "../xml_parse/XmlParseEx.h"

#include "MallDataMgr.h"

CMallDataMgr::CMallDataMgr()
{
	this->LoadWares();
}

CMallDataMgr::~CMallDataMgr()
{

}

unsigned int CMallDataMgr::GetWaresNum(WAREREGION_CLASS eRegion)
{
	unsigned int num = 0;
	TMAPWARES::iterator iter = m_mapWares.find(eRegion);
	if (iter != m_mapWares.end())
	{
		num = iter->second.size();
	}

	return num;
}

unsigned int CMallDataMgr::GetWareID(WAREREGION_CLASS eRegion, unsigned int index)
{
	unsigned int unID = 0;
	TMAPWARES::iterator iter = m_mapWares.find(eRegion);
	if (iter != m_mapWares.end())
	{
		if (index < iter->second.size())
		{
			unID = iter->second[index];
		}
	}

	return unID;
}

void CMallDataMgr::LoadWares()
{
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\Mall.xml");
	if(!pStream)
	{
		Engine_WriteLog("×°ÔØ Mall.xml Ê§°Ü\n");
		return ;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("½âÎö Mall.xml Ê§°Ü\n");
		return;
	}

	TiXmlElement* pWares = doc.FirstChildElement("wares");
	if(pWares)
	{
		TiXmlElement* pInfo = pWares->FirstChildElement("info");
		int nNum = 0;
		if (pInfo->QueryIntAttribute("num",&nNum) != TIXML_SUCCESS)
		{
			return;
		}

		for (int i = 0; i < nNum; ++i)
		{
			char szBuf[MAX_NAMESIZE] = {0};
			_snprintf(szBuf, MAX_NAMESIZE-1, "ware%d", i+1);
			TiXmlElement* pWare = pWares->FirstChildElement(szBuf);
			if (pWare)
			{
				int nID = 0;
				WARES vecItems;
				TiXmlElement* pItem = pWare->FirstChildElement("item");
				while (pItem)
				{
					if (pItem->QueryIntAttribute("id", &nID) != TIXML_SUCCESS)
						continue;

					vecItems.push_back(static_cast<DWORD>(nID));
					pItem = pItem->NextSiblingElement("item");
				}
				m_mapWares.insert(std::make_pair(static_cast<WAREREGION_CLASS>(i), vecItems));
			}
		}
	}
	ClosePackFileForRead(pStream);
}

CMallDataMgr& GetMallDataMgr()
{
	static CMallDataMgr mgr;
	return mgr;
}
