#include "public.h"
#include "Tlhelp32.h"
#include "PEMacro.h"

typedef struct _APIHOOK32_ENTRY
{
	LPCTSTR pszAPIName;     // API名字
	LPCTSTR pszCallerModuleName1; // 被调用的模块名1
	LPCTSTR pszCallerModuleName2; // 被调用的模块名2
	LPCTSTR pszCallerModuleName3; // 被调用的模块名3
	PROC pfnOriginApiAddress; // 原始的函数地址
	PROC pfnDummyFuncAddress; // 新的函数地址
	HMODULE hModCallerModule;//　调用的模块句柄
}APIHOOK32_ENTRY,*PAPIHOOK32_ENTRY; 

/*
PVOID ImageDirectoryEntryToData(
IN LPVOID Base,  　　　　　　// PE结构的基地址
IN BOOLEAN MappedAsImage,   　//映射方法
IN USHORT DirectoryEntry,    // 获取何种信息
OUT PULONG Size              // 获取数据大小
)                             // 返回所指向数据的地址
*/

BOOL _SetApiHookUp(PAPIHOOK32_ENTRY phk)
{
	//PIMAGE_THUNK_DATA pThunk;
	//ULONG size;

	//// 获取指向PE文件中的Import中
	//// IMAGE_DIRECTORY_DESCRIPTOR数组的指针
	//PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
	//	(PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(phk->hModCallerModule,TRUE,
	//	IMAGE_DIRECTORY_ENTRY_IMPORT,&size);

	//if( pImportDesc == NULL)
	//	return FALSE;
	if( phk->hModCallerModule == NULL)
		return FALSE;

	const char  * baseAddress = (const char *)phk->hModCallerModule; 
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) baseAddress;

	if ( IsBadReadPtr(baseAddress, sizeof(PIMAGE_NT_HEADERS)) )
		return 0;

	if ( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return 0;

	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) ( baseAddress + pDOSHeader->e_lfanew);
	if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
		return 0;

	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
		(PIMAGE_IMPORT_DESCRIPTOR) ( baseAddress + 
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress );

	// Bail out if the RVA of the imports section is 0 (it doesn't exist)
	if ( pImportDesc == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader )
		return 0;



	// 查找记录，查看导入表中是否存指定的DLL
	for(;pImportDesc->Name;pImportDesc++)
	{
		LPSTR pszDllName = (LPSTR)((PBYTE)
			phk->hModCallerModule+pImportDesc->Name);


		//Engine_WriteLogF("DEBUG:::::: [%s]\n",pszDllName);

		if( lstrcmpiA(pszDllName,phk->pszCallerModuleName1) == 0 
			|| lstrcmpiA(pszDllName,phk->pszCallerModuleName2) == 0 
			|| lstrcmpiA(pszDllName,phk->pszCallerModuleName3) == 0 )
			break;

	}

	if(pImportDesc->Name == NULL)
		return FALSE;

	//// 寻找我们想要的函数
	//pThunk = (PIMAGE_THUNK_DATA)((PBYTE)phk->hModCallerModule+pImportDesc->FirstThunk); //IAT
	//       for( ; pThunk->u1.Function;pThunk++)
	//{
	//	// ppfn记录了与IAT表项
	//	PROC * ppfn = (PROC * )&pThunk->u1.Function;
	//	if( *ppfn == phk->pfnOriginApiAddress)
	//	{//　如果地址相同，也就是找到了我们想要的函数，
	//	 // 进行修改，将其指向我们所定义的函数
	//		WriteProcessMemory(GetCurrentProcess(),ppfn,
	//			&(phk->pfnDummyFuncAddress),
	//			sizeof(phk->pfnDummyFuncAddress),NULL);
	//		return TRUE;
	//	}
	//}
	return TRUE;
}

BOOL SetWindowsAPIHook(PAPIHOOK32_ENTRY phk)
{
	MEMORY_BASIC_INFORMATION mInfo;
	HMODULE hModHookDLL;
	HANDLE hSnapshot;
	BOOL   bOk;

	MODULEENTRY32 me = {sizeof(MODULEENTRY32)};

	if( phk->pszCallerModuleName1 == NULL && phk->pszCallerModuleName2 == NULL && phk->pszCallerModuleName3 == NULL)
		return FALSE;
	//if(phk->pszAPIName == NULL || phk->pszCallerModuleName == NULL
	// || phk->pfnOriginApiAddress == NULL)
	// return FALSE;

	if(phk->hModCallerModule == NULL)
	{
		VirtualQuery(_SetApiHookUp,&mInfo,sizeof(mInfo));
		hModHookDLL = (HMODULE)mInfo.AllocationBase;

		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
		bOk = Module32First(hSnapshot,&me);
		while(bOk)
		{
			if(me.hModule == hModHookDLL)
			{
				phk->hModCallerModule = me.hModule;
				if( _SetApiHookUp(phk))
					return TRUE;
				break;
			}
			bOk = Module32Next(hSnapshot,&me);
		}
		phk->hModCallerModule = NULL;
		return FALSE;
	}
	else
		return _SetApiHookUp(phk);
	return FALSE;
} 

BOOL UnhookWindowsAPIHooks(PAPIHOOK32_ENTRY lpHk)
{
	PROC temp;

	temp = lpHk->pfnOriginApiAddress;
	lpHk->pfnOriginApiAddress = lpHk->pfnDummyFuncAddress;
	lpHk->pfnDummyFuncAddress = temp;

	return SetWindowsAPIHook(lpHk);
}

//PROC lpAddr = MessageBoxA;
//BOOL CHECKZTIAT(int i)
//{
//      APIHOOK32_ENTRY pe;
//      pe.pszAPIName = NULL; // MessageBoxA
//      pe.pszCallerModuleName1 = "zthook.dll";
//      pe.pszCallerModuleName2 = "ptdll.dll";
//      pe.pszCallerModuleName3 = "makewaydll.dll";
//      pe.pfnOriginApiAddress = NULL;
//      pe.pfnDummyFuncAddress = NULL;
//      pe.hModCallerModule = NULL;
//
//
//      //lpAddr = MessageBoxA;
//      // 挂钩
//      BOOL bHooked =  SetWindowsAPIHook(&pe);
//      UnhookWindowsAPIHooks(&pe);
//
//      return bHooked;
//
//}

#define CHECKZTIAT	      APIHOOK32_ENTRY pe;\
	pe.pszAPIName = NULL; \
	pe.pszCallerModuleName1 = "zthook.dll";\
	pe.pszCallerModuleName2 = "ptdll.dll";\
	pe.pszCallerModuleName3 = "makewaydll.dll";\
	pe.pfnOriginApiAddress = NULL;\
	pe.pfnDummyFuncAddress = NULL;\
	pe.hModCallerModule = NULL;\
	BOOL bHooked =  SetWindowsAPIHook(&pe);\
	UnhookWindowsAPIHooks(&pe);\
	return bHooked;


FUNCTION_ARRAY(PFUNCTION,CheckZtiat)
FUNCTION_DEFINE1_R_8(CheckZtiat,int,i,BOOL,FALSE,CHECKZTIAT)
