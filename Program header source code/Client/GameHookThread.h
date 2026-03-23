#ifndef _GAMEHOOKTHREAD_H
#define _GAMEHOOKTHREAD_H

unsigned __stdcall GameHookThread( void * );

inline CThread * GetHookThread()
{
	extern CThread  g_HookThread;
	return &g_HookThread;
}

#endif