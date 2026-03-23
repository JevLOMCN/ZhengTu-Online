#include "public.h" 


#ifdef _REGISTER_ERROR
__declspec(thread) stCallStackInfo g_callStack[256];
__declspec(thread) int			  g_callStackIndex=-1;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param code : ÃèÊö
 * \param pError : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int RegErrorCallStack(DWORD code,LPEXCEPTION_POINTERS pError)
{
	//Engine_WriteLogF("GetExceptionCode() = 0x%x\n",code);
	if(pError)
	{
		Engine_WriteLogF("ExceptionCode = 0x%x\n",pError->ExceptionRecord->ExceptionCode);
		Engine_WriteLogF("ExceptionFlags = 0x%x\n",pError->ExceptionRecord->ExceptionFlags);
		Engine_WriteLogF("ExceptionAddress = 0x%x\n",pError->ExceptionRecord->ExceptionAddress);
		for(int i=0;i<pError->ExceptionRecord->NumberParameters;++i)
			Engine_WriteLogF("ExceptionInformation[%u] = 0x%x\n",i,pError->ExceptionRecord->ExceptionInformation[i]);
	}
	Engine_WriteLog("CallStack:\n");
	for(int i=g_callStackIndex;i>=0;--i)
	{
		Engine_WriteLogF("%s(%d) %s\n",g_callStack[i].filename,g_callStack[i].lineno,g_callStack[i].sTime);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

