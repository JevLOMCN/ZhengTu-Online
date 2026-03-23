#include "public.h"
#include "./UserLocalInfo.h"
#include "./GameAppation.h"

CUserLocalInfo::CUserLocalInfo()
{
	m_curZone = 0;
	m_curVersion = 0;
	//m_mapUnionName.insert( int_string_Pair( 0, "a苹果帮" ) );
	//m_mapUnionName.insert( int_string_Pair( 109, "c苹果帮11" ) );
	//m_mapSpetName.insert( int_string_Pair( 0, "a苹果家" ) );
}

CUserLocalInfo::~CUserLocalInfo()
{
}
void CUserLocalInfo::Clear()
{
	m_mapUnionName.erase(m_mapUnionName.begin( ),m_mapUnionName.end( ));
	m_mapSpetName.erase(m_mapSpetName.begin( ),m_mapSpetName.end( ));
	curRequesetUnionId.erase(curRequesetUnionId.begin( ),curRequesetUnionId.end( ));
	curRequesetSpetId.erase(curRequesetSpetId.begin( ),curRequesetSpetId.end( ));
}
int CUserLocalInfo::GetCurZone()
{
	stGameConfig config = *((CGameAppation*)Engine_GetAppation())->GetConfig();
	m_curZone = config.nZone;
	return m_curZone;
}

DWORD CUserLocalInfo::GetCurVersion()
{
	return m_curVersion;
}

void CUserLocalInfo::SetCurVersion(DWORD nCurVersion)
{
	m_curVersion = nCurVersion;
}

bool CUserLocalInfo::SaveUserLocalFile(const char* pszFileName)
{
	FUNCTION_BEGIN;
	
	char temp[256]; 
	if(pszFileName == NULL)
	{
		return false ;
	}
	TiXmlDocument doc;
	if(!doc.LoadFile(pszFileName))
	{
		doc.InsertEndChild(TiXmlDeclaration("1.0","gb2312",""));
	}
	TiXmlElement* proot = XML_FindAndAddChildNode(&doc,"root");
	if( NULL != proot )
	{		
		//////////////////////////////////////////////////////////////////////////
		//文档版本号
		//////////////////////////////////////////////////////////////////////////
		TiXmlElement* pzone = proot->FirstChildElement("zone");
		if (NULL == pzone)
		{
			pzone = proot->InsertEndChild( TiXmlElement("zone") )->ToElement();
			XML_SetAttribute(pzone,"id",GetCurVersion(),10);
		}
		//////////////////////////////////////////////////////////////////////////
		//获得当前活动的区块,将map里面的信息写入该区块
		//////////////////////////////////////////////////////////////////////////
		if( NULL != pzone )
		{
			XML_SetAttribute(pzone,"id",GetCurVersion(),10);
			pzone->Clear();
			for(m_pIter = m_mapUnionName.begin( );m_pIter != m_mapUnionName.end( );m_pIter++)
			{
				TiXmlElement* punion = pzone->InsertEndChild( TiXmlElement("union") )->ToElement();
				XML_SetAttribute(punion,"id",m_pIter->first,10);
				sprintf(temp,"%s.",m_pIter->second.c_str());
				XML_SetNodeText(punion,temp);
			}
			for(m_pIter = m_mapSpetName.begin( );m_pIter != m_mapSpetName.end( );m_pIter++)
			{
				TiXmlElement* pspet = pzone->InsertEndChild( TiXmlElement("spet") )->ToElement();
				XML_SetAttribute(pspet,"id",m_pIter->first,10);
				sprintf(temp,"%s.",m_pIter->second.c_str());
				XML_SetNodeText(pspet,temp);
			}
		}
	}
	if( !doc.SaveFile(pszFileName) )
	{
		Assert(0);
		TRACE("SaveFile:%s error\n",pszFileName);		
	}	
	else
	{
		//保存成功则...
		return true;
	}	
	
	return false;
	FUNCTION_END;
}
bool CUserLocalInfo::LoadUserLocalInfo(const char* pszFileName)
{	
	FUNCTION_BEGIN;
	
	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("",pszFileName);
	if(!pStream)
	{
		Engine_WriteLog("装载 用户本地信息文件 失败\n");
		return false;
	}
	if(!LoadXMLFromStream(pStream,doc))
	{
		CloseResFile(pStream);
		Engine_WriteLog("解析 用户本地信息文件 失败\n");
		return false;
	}
	TiXmlElement* proot = doc.FirstChildElement("root");
	if(proot)
	{
		TiXmlElement* pzone = proot->FirstChildElement("zone");
		int id = -1;
		std::string temp;
		while(pzone)
		{
			const char* pzoneid = pzone->Attribute("id");
			id = atoi(pzoneid);
			m_curVersion = id;
			//if( id == GetCurZone())//当前的区
			{	
				TiXmlElement* punion = pzone->FirstChildElement("union");
				while (punion)
				{
					const char* punionid = punion->Attribute("id");
					if(punionid)
					{
						id = atoi(punionid);
						temp = XML_GetNodeText(punion);
						temp.resize(temp.length()-1);
						m_mapUnionName.insert( int_string_Pair( id, temp ) );
					}
					punion = punion->NextSiblingElement("union");
				}
				//
				TiXmlElement* pspet = pzone->FirstChildElement("spet");
				while (pspet)
				{
					const char* pspetid = pspet->Attribute("id");
					if(pspetid)
					{
						id = atoi(pspetid);
						temp = XML_GetNodeText(pspet);
						temp.resize(temp.length()-1);
						m_mapSpetName.insert( int_string_Pair( id, temp ) );
					}
					pspet = pspet->NextSiblingElement("spet");
				}
				//
				break;
			}
			pzone = pzone->NextSiblingElement("zone");
		}
	}
	ClosePackFileForRead(pStream);

	return true;
	FUNCTION_END;
}