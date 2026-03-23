#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <list>

#pragma warning( disable : 4244 ) 

#ifdef _USEDLL
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API 
#endif

