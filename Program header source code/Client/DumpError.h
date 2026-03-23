
#ifndef _X_DUMP_ERROR
#define _X_DUMP_ERROR

typedef void (*DumpStackCallback)(const char* szFormat,...);

void appInitStackWalking();
INT appWalkStack(LPEXCEPTION_POINTERS ExceptionInfo, DWORD64 *BackTrace, DWORD& MaxDepth);
DWORD appDumpStack(const DWORD64* BackTrace, DWORD MaxDepth, DumpStackCallback Callback);
INT CreateMiniDump( LPEXCEPTION_POINTERS ExceptionInfo,DWORD ExceptionCode,DumpStackCallback Callback );
INT SimpleReportError(LPEXCEPTION_POINTERS ExceptionInfo ,DWORD ExcptionCode);
INT SendReportError(LPEXCEPTION_POINTERS ExceptionInfo ,DWORD ExcptionCode);

#endif