/**
 * \file
 * \version  $Id: zVersion.h c $
 * \author  
 * \date 
 * \brief 得到服务器版本信息，定义和实现均在本文件中
 *
 * 
 */

#ifndef _zVersion_h_
#define _zVersion_h_

#ifndef MAJOR_VERSION
#define MAJOR_VERSION 0
#endif

#ifndef MINOR_VERSION
#define MINOR_VERSION 0
#endif

#ifndef MICRO_VERSION
#define MICRO_VERSION 0
#endif

#ifndef BUILD_STRING
#define BUILD_STRING "build041224.0000"
#endif

#ifdef	_ALL_SUPER_GM
#define DEBUG_STRING	"_ALL_SUPER_GM"
#else
#define DEBUG_STRING	""
#endif

#define _TY(x) #x
#define _S(x) _TY(x)

#ifndef VS
#define VERSION_STRING	_S(MAJOR_VERSION)"."_S(MINOR_VERSION)"."_S(MICRO_VERSION)
#else
#define VERSION_STRING _S(VS)
#endif

#endif

