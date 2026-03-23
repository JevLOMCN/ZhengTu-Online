#ifndef _FIND_DLL_H
#define _FIND_DLL_H

#define CHECK_RET_ADDR {\
	{__asm cmp dword ptr [ebp+4],10000000h \
	 __asm jl  _check_retaddr_ok } \
	 stDirtyUserUserCmd cmd;\
	 cmd.type = DIRTYUSER_DLL_CALL;\
	 SEND_USER_CMD(cmd) \
_check_retaddr_ok: \
	; }

//#define CHECK_RET_ADDR

//#define CHECK_RET_ADDR {\
//	{__asm cmp dword ptr [ebp+4],10000000h \
//	 __asm jl  _check_retaddr_ok } \
//	 exit(0);\
//_check_retaddr_ok: \
//	; }

#if defined _DEBUG || defined RELEASE_TEST || defined _AUTOPLAY
//#if 0
#define CHECK_DEBUGGER 
#define CHECK_DEBUGGER_INIT 

#else

//#define CHECK_DEBUGGER {\
//	{__asm mov         eax,dword ptr fs:[00000018h] \
//	 __asm mov         eax,dword ptr [eax+30h] \
//	 __asm movzx       eax,byte ptr  [eax+2] \
//	 __asm cmp         eax,0 \
//	 __asm je		   _check_debug_ok } \
//	 stDirtyUserUserCmd cmd; \
//	 cmd.type = DIRTYUSER_DEBUGGER; \
//	 SEND_USER_CMD(cmd); \
//_check_debug_ok:\
//	; }

#define CHECK_DEBUGGER 

//#define CHECK_DEBUGGER_INIT { \
//	{__asm mov        eax,dword ptr fs:[00000018h] \
//	__asm mov         eax,dword ptr [eax+30h] \
//	__asm movzx       eax,byte ptr  [eax+2] \
//	__asm cmp         eax,0 \
//	__asm je		  _check_debug_init_ok } \
//	exit(0); \
//_check_debug_init_ok: \
//	; }

#define CHECK_DEBUGGER_INIT 

#endif

#endif