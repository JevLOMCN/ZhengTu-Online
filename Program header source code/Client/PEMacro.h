#pragma once

#include "MulFunction.h"

//////////////////////////////////////////////////////////////////////
typedef BOOL (*PFUNCTION)(int);
FUNCTION_DECLARE1_R_8(CheckZtiat,int,BOOL)
EXTERN_FUNCTION_ARRAY(PFUNCTION,CheckZtiat)
#define CallCheckZtiat(i)	FUNCTION_CALL1_R(CheckZtiat,i,BOOL)

//////////////////////////////////////////////////////////////////////

 