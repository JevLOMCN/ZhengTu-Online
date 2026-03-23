//#include "../engine/include/engine.h"
#include "public.h"
#include "./DumpError.h"
#include "command.h"
#include "./NetAutoConnect.h"
#include <Dbghelp.h>
//#pragma comment(lib ,"Dbghelp.lib")

static char* sStrDumpStackCallback = NULL;

typedef BOOL
(WINAPI
*tFMiniDumpWriteDump)(
				  IN HANDLE hProcess,
				  IN DWORD ProcessId,
				  IN HANDLE hFile,
				  IN MINIDUMP_TYPE DumpType,
				  IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
				  IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
				  IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
				  );

static tFMiniDumpWriteDump FMiniDumpWriteDump = NULL;

typedef BOOL
(WINAPI
*tFSymInitialize)(
			  IN HANDLE   hProcess,
			  IN PSTR     UserSearchPath,
			  IN BOOL     fInvadeProcess
			  );

static tFSymInitialize FSymInitialize = NULL;

typedef BOOL
(WINAPI
*tFStackWalk64)(
			DWORD                             MachineType,
			HANDLE                            hProcess,
			HANDLE                            hThread,
			LPSTACKFRAME64                    StackFrame,
			PVOID                             ContextRecord,
			PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
			PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
			PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
			PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
			);

static tFStackWalk64 FStackWalk64 = NULL;

typedef BOOL
(WINAPI
*tFSymFromAddr)(
			IN  HANDLE              hProcess,
			IN  DWORD64             Address,
			OUT PDWORD64            Displacement,
			IN OUT PSYMBOL_INFO     Symbol
			);

tFSymFromAddr FSymFromAddr = NULL;

typedef BOOL
(WINAPI
*tFSymGetLineFromAddr64)(
					 IN  HANDLE                  hProcess,
					 IN  DWORD64                 qwAddr,
					 OUT PDWORD                  pdwDisplacement,
					 OUT PIMAGEHLP_LINE64        Line64
					 );

tFSymGetLineFromAddr64 FSymGetLineFromAddr64 = NULL;

typedef DWORD
(WINAPI
*tFSymGetOptions)(
			  VOID
			  );

static tFSymGetOptions FSymGetOptions = NULL;

typedef DWORD
(WINAPI 
*tFSymSetOptions)(
			  IN DWORD   SymOptions
			  );

static tFSymSetOptions FSymSetOptions = NULL;

typedef PVOID
(WINAPI 
*tFSymFunctionTableAccess64)(
						 HANDLE  hProcess,
						 DWORD64 AddrBase
						 );

static tFSymFunctionTableAccess64 FSymFunctionTableAccess64 = NULL;

typedef DWORD64
(WINAPI
*tFSymGetModuleBase64)(
				   IN  HANDLE              hProcess,
				   IN  DWORD64             qwAddr
				   );

static tFSymGetModuleBase64 FSymGetModuleBase64 = NULL;

static HMODULE sDbgHelpLib = NULL;

static bool InitDbgHelpLib()
{
	sDbgHelpLib = LoadLibrary("dbghelp.dll");
	if(NULL == sDbgHelpLib)
		return false;

	FMiniDumpWriteDump = (tFMiniDumpWriteDump)GetProcAddress(sDbgHelpLib,"MiniDumpWriteDump");
	FSymInitialize = (tFSymInitialize)GetProcAddress(sDbgHelpLib,"SymInitialize");
	FStackWalk64 = (tFStackWalk64)GetProcAddress(sDbgHelpLib,"StackWalk64");
	FSymFromAddr = (tFSymFromAddr)GetProcAddress(sDbgHelpLib,"SymFromAddr");
	FSymGetLineFromAddr64 = (tFSymGetLineFromAddr64)GetProcAddress(sDbgHelpLib,"SymGetLineFromAddr64");
	FSymGetOptions = (tFSymGetOptions)GetProcAddress(sDbgHelpLib,"SymGetOptions");
	FSymSetOptions = (tFSymSetOptions)GetProcAddress(sDbgHelpLib,"SymSetOptions");
	FSymFunctionTableAccess64 = (tFSymFunctionTableAccess64)GetProcAddress(sDbgHelpLib,"SymFunctionTableAccess64");
	FSymGetModuleBase64 = (tFSymGetModuleBase64)GetProcAddress(sDbgHelpLib,"SymGetModuleBase64");

	return (FMiniDumpWriteDump && 
			FSymInitialize && 
			FStackWalk64 && 
			FSymFromAddr &&
			FSymGetLineFromAddr64 && 
			FSymGetOptions && 
			FSymSetOptions && 
			FSymFunctionTableAccess64 &&
			FSymGetModuleBase64);
}

static void FreeDbgHelpLib()
{
	if(NULL != sDbgHelpLib){
		FreeLibrary(sDbgHelpLib);
		sDbgHelpLib = NULL;
	}

	FMiniDumpWriteDump = NULL;
	FSymInitialize = NULL;
	FStackWalk64 = NULL;
	FSymFromAddr = NULL;
	FSymGetLineFromAddr64 = NULL;
	FSymGetOptions = NULL; 
	FSymSetOptions = NULL;
	FSymFunctionTableAccess64 = NULL;
	FSymGetModuleBase64 = NULL;
}

static void sDumpStackCallback(const char* szFormat,...)
{
	va_list args;
	va_start( args, szFormat );
	vsprintf(sStrDumpStackCallback,szFormat,args);
	va_end(args);
	sStrDumpStackCallback += strlen(sStrDumpStackCallback);
}

INT SimpleReportError(LPEXCEPTION_POINTERS ExceptionInfo ,DWORD ExcptionCode)
{
	char* msg = new char[65535];
	sStrDumpStackCallback = msg;
	char szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	char* p = strrchr(szFileName,'.');
	if(p) *p = 0;

	INT retval = CreateMiniDump(ExceptionInfo,ExcptionCode,sDumpStackCallback);
	appDump << msg;
	((CFileLog&)appDump).close();

	char* szMessage = new char[65535];
	sprintf(szMessage,"游戏遇到问题需要关闭，请把 %s.dmp 和 %s.log 文件发给游戏开发者(客户端程序)，以便解决问题！\n",szFileName,szFileName);
	strcpy(szMessage+strlen(szMessage),msg);
	MessageBox(NULL,szMessage,"程序头",MB_OK|MB_ICONERROR);

	delete [] msg;
	delete [] szMessage;
	return 	retval;
}

INT SendReportError(LPEXCEPTION_POINTERS ExceptionInfo ,DWORD ExcptionCode)
{

	char* msg = new char[65535];
	sStrDumpStackCallback = msg;

	INT retval = CreateMiniDump(ExceptionInfo,ExcptionCode,sDumpStackCallback);
	if(retval == 0)
		return 0;
	appDump << msg;
	((CFileLog&)appDump).close();

	extern void ShowErrorDlg(const char* msg);
	ShowErrorDlg(msg);

	delete [] msg;

	return 	retval;
}

INT CreateMiniDump( LPEXCEPTION_POINTERS ExceptionInfo,DWORD ExceptionCode, DumpStackCallback Callback )
{
	if(!InitDbgHelpLib())
		return 0;

#if defined _DEBUG || defined RELEASE_TEST || defined _AUTOPLAY 
	File f;
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	TCHAR* p = _tcsrchr(szFileName,'\\');
	_tcscpy(szFileName,p+1);
	p = _tcsrchr(szFileName,'.');
	if(p) _tcscpy(p,".dmp");
	if(File::Ok!=f.open(szFileName,File::Write)){
		FreeDbgHelpLib();
		return 0;
	}

	HANDLE FileHandle	= f.getHandle();

	// Write a minidump.
	if( FileHandle )
	{
		MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo;

		DumpExceptionInfo.ThreadId			= GetCurrentThreadId();
		DumpExceptionInfo.ExceptionPointers	= ExceptionInfo;
		DumpExceptionInfo.ClientPointers	= true;

		FMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), FileHandle, MiniDumpNormal, &DumpExceptionInfo, NULL, NULL );
		f.close();
	}
#endif

	char szDate[16];
	char szTime[16];
	_strdate(szDate);
	_strtime(szTime);
	Callback("%s %s\n",szDate,szTime);
	Callback("version %d\n",ZTGAME_VERSION);
	Callback("Exception Code 0x%08x\n",ExceptionCode);
	Callback("Exception Flags 0x%08x\n",ExceptionInfo->ExceptionRecord->ExceptionFlags);
	Callback("Exception Address 0x%08x\n",ExceptionInfo->ExceptionRecord->ExceptionAddress);
	Callback("Exception Call Stack:\n");
	// Walk the stack.
	DWORD64 BackTrace[64];
	appInitStackWalking();
	DWORD depth = count_of(BackTrace);
	const INT Result = appWalkStack( ExceptionInfo, BackTrace, depth );
	appDumpStack( BackTrace, depth, Callback );

	FreeDbgHelpLib();
	return Result;
}

void appInitStackWalking()
{
	static bool Init = false;
	if ( !Init )
	{
		Init = true;
		FSymInitialize( GetCurrentProcess(), ".", 1 );
	}
}

INT appWalkStack(LPEXCEPTION_POINTERS ExceptionInfo, DWORD64 *BackTrace, DWORD& MaxDepth)
{
	// Grab thread context from the exception.
	CONTEXT* ContextRecord = ExceptionInfo->ContextRecord;

	// Initialize the stackframe.
	STACKFRAME64 StackFrame;
	memset( &StackFrame, 0, sizeof(StackFrame));
	StackFrame.AddrPC.Offset = ContextRecord->Eip;
	StackFrame.AddrFrame.Offset = ContextRecord->Ebp;
	StackFrame.AddrStack.Offset = ContextRecord->Esp;
	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrFrame.Mode = AddrModeFlat;
	StackFrame.AddrStack.Mode = AddrModeFlat;

	// Load symbols.
	const HANDLE Process = GetCurrentProcess();
	const HANDLE Thread = GetCurrentThread();

	// Walk the stack.
	DWORD CurDepth = 0;
	while ( CurDepth < MaxDepth )
	{
		if ( !FStackWalk64( IMAGE_FILE_MACHINE_I386, Process, Thread, &StackFrame, NULL, NULL, FSymFunctionTableAccess64, FSymGetModuleBase64, NULL ) )
		{
			break;
		}

		BackTrace[CurDepth++] = StackFrame.AddrPC.Offset;
	}
	
	int iDepthNum = CurDepth;
	for ( ; CurDepth < MaxDepth ; ++CurDepth )
	{
		BackTrace[CurDepth] = 0;
	}

	MaxDepth = iDepthNum;

	return EXCEPTION_EXECUTE_HANDLER;

}

DWORD appDumpStack(const DWORD64* BackTrace, DWORD MaxDepth, DumpStackCallback Callback)
{
	if ( !Callback )
	{
		return 0;
	}

	// Set symbol options.
	DWORD SymOptions = FSymGetOptions();
	SymOptions |= SYMOPT_LOAD_LINES;
	SymOptions |= SYMOPT_UNDNAME;
	SymOptions |= SYMOPT_EXACT_SYMBOLS;
	FSymSetOptions( SymOptions );

	// Initialize symbol structure.
	const int MAX_SYMBOL_NAME_LENGTH = 256;
	BYTE Buffer[sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LENGTH ];
	SYMBOL_INFO* Symbol = (SYMBOL_INFO*)( (void*)Buffer );
	Symbol->SizeOfStruct = sizeof( SYMBOL_INFO );
	Symbol->MaxNameLen = MAX_SYMBOL_NAME_LENGTH;

	// Initialize line structure.
	IMAGEHLP_LINE64 Line;
	Line.SizeOfStruct = sizeof( Line );

	// Load symbols.
	const HANDLE Process = GetCurrentProcess();
	//SymInitialize( Process, ".", 1 );

	// Dump symbol info.
	DWORD NumSuccess = 0;
	DWORD64 Offset64;
	DWORD Offset;
	for ( DWORD i = 0 ; i < MaxDepth ; ++i )
	{
		if ( BackTrace[i] == 0 )
		{
			break;
		}

		if ( FSymFromAddr( Process, BackTrace[i], &Offset64, Symbol ) &&
			FSymGetLineFromAddr64( Process, BackTrace[i], &Offset, &Line ) )
		{
			++NumSuccess;
			Callback( "0x%08x    func %-45s    Line %05i of %s\r\n",(DWORD)BackTrace[i], Symbol->Name, (int)Line.LineNumber, Line.FileName );
		}else{
			Callback( "0x%08x\r\n" ,(DWORD)BackTrace[i]);
		}
	}

	return NumSuccess;
}
