#pragma once

#include "./command.h"

class CTask
{
public:
	//任务变量
	struct stTaskDataVar
	{
		BYTE name[stQuestVarsUserCmd::MAX_NSIZE];
		BYTE value[stQuestVarsUserCmd::MAX_VSIZE];
	};
	typedef std::vector<stTaskDataVar> TaskVarList;
	typedef TaskVarList::iterator TaskVarListIt;

	//任务
	struct stTaskData
	{
		~stTaskData()
		{
			TaskData.clear();
		}

		DWORD		TaskID;
		BYTE		Taskoff;
		int			TaskLevel;
		std::string	TaskTitle;
		struct tm 	TaskStartTime;
		std::string	TaskInfo;
		TaskVarList	TaskData;
		bool		TaskShow;
		int			TaskState;

		stTaskData()
		{
			TaskState = 1;
		}

		bool UpdateTaskData(const char*name,const char* value);

		bool bIsFinish()
		{
			return ( TaskState == -3 );
			return false;
		}

		std::string GetTaskName()
		{
			if(bIsFinish())
			{
				return TaskTitle + "  (完成)";
			}
			return TaskTitle;
		}

		std::string GetValue(const char* name)
		{
			for( TaskVarListIt&	it = TaskData.begin(); it != TaskData.end(); ++it )
			{
				if (0 == strcmp(name,(char *)(it->name) ))
				{
					return (char *)(it->value);
				}
			}
			return std::string("0");
		}
	};

	typedef std::vector<stTaskData> TaskList;	
	typedef TaskList::iterator	TaskListIt;

public:	
	CTask();
	~CTask();
	static bool bIsMapTask(DWORD dwTask){	return ( dwTask > 100000 );	}
	static bool bIsMainTask(DWORD dwTask) { return ( dwTask < 10000 ); }
	static bool bIsSwitchTask(DWORD dwTask) { return ( dwTask >= 10000 && dwTask <= 100000); }

	inline bool bIsMapTaskFinished( DWORD dwTask )
	{
		TaskListIt	it = GetTaskItByID(dwTask);
		if( it != m_taskList.end() ) return bIsMapTask(it->TaskID);
		return false;
	}

	TaskListIt GetTaskItByID(DWORD dwTask)
	{
		for( TaskListIt	it = m_taskList.begin(); it != m_taskList.end(); ++it )
		{
			if ( it->TaskID == dwTask ) return it;
		}
		return m_taskList.end();
	}
	bool bIsTaskExist(DWORD dwTask)
	{
		return ( GetTaskItByID(dwTask) != m_taskList.end() );
	}
	int GetTaskLevel(DWORD dwTask) //-1 is error
	{
		TaskListIt it = GetTaskItByID(dwTask);
		if (it != m_taskList.end()) return it->TaskLevel;
		return -1;
	}

	bool UpdateQuestVars(stQuestVarsUserCmd* pcmd);
	bool AddQuestVars(stQuestInfoUserCmd* pcmd);
	std::string GetTaskValue(DWORD dwTask,const char* name);

	void RemoveTask(DWORD dwTask);
	void GetTaskByXml();
	std::string GetTaskShow(DWORD dwTask);
	TaskList* GetTask() { return &m_taskList; }
private:
	TaskList m_taskList;
};

int GetLevelFromXML(const char* strXML);
//bool GetTextFromXML(const char* strXML,std::string& strResult,int &level,std::vector<DWORD> *data = NULL,DWORD defaultColor = -1);