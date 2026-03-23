#include "../include/engine.h"
#include "../include/lookaside_alloc.h"
_STD_BEGIN 
const DWORD c_dwLookasideAllocFreeTime = 10 * 60 * 1000;
size_t gLookasideAllocSize = 0;
_STD_END