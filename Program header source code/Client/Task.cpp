#include "public.h"
#include ".\Task.h"
#include "../xml_parse/XmlParseEx.h"
#include "GameScene.h"
#include "GameGuiManager.h"
#include "GuiMain.h"
#include "NationalMapDialog.h"
#include "./Country.h"

const DWORD defaultMessageColor = COLOR_ARGB(255,255,255,20);

int GetLevelFromXML(const char* strXML)
{
	int level = 0;
	TiXmlDocument doc;
	doc.Parse(strXML);
	if(doc.Error()) return level;
	TiXmlElement * body = doc.FirstChildElement("body");
	if(!body) return false;

	TiXmlElement* diffculty = body->FirstChildElement("diffcult");
	if(diffculty)
	{
		if(diffculty->QueryIntAttribute("level",&level) != TIXML_SUCCESS)
			level = 0;
		else
		{
			return level;
		}
		if ( level == 0 )
		{
			int taskID = -1;
			if(diffculty->QueryIntAttribute("task",&taskID) != TIXML_SUCCESS)
				taskID = -1;
			const char* taskVar = diffculty->Attribute("var");
			if(taskID != -1 && taskVar)
			{
				std::string strTaskVar = GetScene()->GetTaskVariable(taskID,taskVar);				
				level = strtoul(strTaskVar.c_str(),NULL,10);
				return level;
			}
		}
	}
	return 0;
}

std::string GetMessageFromXML(const char* strXML,DWORD &dwColor)
{
	std::string str = "";
	TiXmlDocument doc;
	doc.Parse(strXML);
	if(doc.Error()) return str;
	TiXmlElement * body = doc.FirstChildElement("body");
	if(!body) return str;

	TiXmlElement* message = body->FirstChildElement("message");
	if(message)
	{
		dwColor = XML_GetColorAttribute(message,"color",defaultMessageColor);

		TiXmlElement* n = message->FirstChildElement("n");
		while(n)
		{
			int taskID = -1;
			str += XML_GetNodeText(n);
			if(n->QueryIntAttribute("task",&taskID) != TIXML_SUCCESS)
				taskID = -1;
			const char* taskVar = n->Attribute("var");
			std::string strTaskVar;
			if(taskID != -1 && taskVar)
			{
				strTaskVar = GetScene()->GetTaskVariable(taskID,taskVar);
				if(strTaskVar == "0")
				{
					return std::string("");
				}
				str += strTaskVar;
			}
			n = n->NextSiblingElement("n");
		}
	}
	return str;
}

bool CTask::stTaskData::UpdateTaskData(const char*name,const char* value)
{
	if ( 0 == strcmp(name,"state") )
	{
		TaskState = strtoul(value,NULL,10);
		if ( TaskState == 5 )
		{
			TaskShow = false;
		}
	}
	if ( 0 == strcmp(name,"clock") )
	{
		DWORD time = strtoul(value,NULL,10);
		if( GetGameGuiManager()->m_guiMain )
		{
			GetGameGuiManager()->m_guiMain->AddTaskTimeMessage(TaskID,TaskTitle.c_str(),time);
		}
	}
	for( TaskVarListIt&	it = TaskData.begin(); it != TaskData.end(); ++it )
	{
		if (0 == strcmp(name,(char *)(it->name) ))
		{
			memcpy( it->value,value, stQuestVarsUserCmd::MAX_VSIZE );
			return true;
		}
	}
	stTaskDataVar	data;
	memset( &data, 0, sizeof(stTaskDataVar) );
	memcpy( data.name,name,stQuestVarsUserCmd::MAX_NSIZE );
	memcpy( data.value,value, stQuestVarsUserCmd::MAX_VSIZE );
	TaskData.push_back(data);
	return false;
}


CTask::CTask(void)
{
}

CTask::~CTask(void)
{
	m_taskList.clear();
}


bool CTask::UpdateQuestVars(stQuestVarsUserCmd* pcmd)
{
	if(!pcmd) return false;
//
//#ifdef _DEBUG
//	for( int i=0; i < pcmd->count; i++ )
//	{
//		TRACE("name:[%s] value:[%s]\n",(char *)pcmd->vars_list[i].name,(char *)pcmd->vars_list[i].value);
//	}	
//#endif
//	
	TaskListIt it = GetTaskItByID(pcmd->id);
	if(it!= m_taskList.end())
	{
		for( int i=0; i < pcmd->count; i++ )
		{
			it->UpdateTaskData((char *)pcmd->vars_list[i].name,(char *)pcmd->vars_list[i].value);
		}		
		
		DWORD color = defaultMessageColor;
		std::string str = GetMessageFromXML(it->TaskInfo.c_str(),color);


		if( str.size() > 0 )
		{
			GetGameGuiManager()->AddClientTaskMessage(str.c_str(),color);	
		}

		return true;
	}
	else
	{
		stTaskData	data;
		data.TaskID = pcmd->id;
		data.Taskoff = 0;
		data.TaskShow = false;
		data.TaskTitle = "";
		data.TaskInfo = "";
		data.TaskLevel = 0;
		for( int i=0; i < pcmd->count; i++ )
		{
			data.UpdateTaskData((char *)pcmd->vars_list[i].name,(char *)pcmd->vars_list[i].value);
		}	
		m_taskList.push_back( data );
	}
	
	return false;
}

bool CTask::AddQuestVars(stQuestInfoUserCmd* pcmd)
{
	if( bIsTaskExist(pcmd->id) ) return false;
	
	if( pcmd->name[0] )
	{
		stTaskData	data;
		data.TaskID = pcmd->id;	
		data.Taskoff = 0;
		data.TaskShow = true;
		data.TaskTitle = (char*)pcmd->name;
		time_t	tmpt = pcmd->start;
		data.TaskStartTime = *gmtime( &tmpt );
		data.TaskInfo = (char*)pcmd->info;
		data.TaskLevel = GetLevelFromXML(data.TaskInfo.c_str());

		m_taskList.push_back( data );

		if( bIsMapTask(pcmd->id) && GetGameGuiManager()->m_guiNationalMap )
		{
			GetGameGuiManager()->m_guiNationalMap->UpdateProvinceMap();
		}
	}
	return true;
}

void CTask::RemoveTask(DWORD dwTask)
{
	TaskListIt it = GetTaskItByID(dwTask);
	if(it != m_taskList.end())
	{
		m_taskList.erase(it);
	}
}


std::string CTask::GetTaskShow(DWORD dwTask)
{
	TaskListIt it = GetTaskItByID(dwTask);
	if(it != m_taskList.end())
	{
		return it->TaskInfo;
	}
	return std::string("");
}

std::string CTask::GetTaskValue(DWORD dwTask,const char* name)
{
	TaskListIt it = GetTaskItByID(dwTask);
	if (it != m_taskList.end())
	{
		std::string str = it->GetValue(name);
		if ( 0 == strcmp(name,"country") || 0 == strcmp(name,"r_country") )
		{
			unsigned long countryid = strtoul(str.c_str(),NULL,10);

			char szCountryName [MAX_NAMESIZE];
			if( CCountry::Instance()->GetCountryName(countryid,szCountryName))
			{
				return std::string(szCountryName);
			}

			Engine_WriteLogF("任务-->没有找到[%d]对应的国家",countryid);
			return std::string("");
		}
		else if( str == "null" || str == "NULL" )
		{
			return std::string("");
		}
		return str;
	}
	return std::string("0");
}