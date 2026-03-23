/********************************************************************
	创建时间:	2004/10/31   15:13
	文件名: 	streamtypes.h
	文件类型:	h
	作者:		$$$03公司
	
	用途:	
*********************************************************************/

#ifndef _XZ_STREAMTYPE_H
#define _XZ_STREAMTYPE_H

/*
#include <windows.h>
#include <assert.h>
#ifndef AssertFatal

#ifdef _DEBUG
#define AssertFatal(exp,msg) (void)( (exp) || (_assert(msg, __FILE__, __LINE__), 0) )
#else
#define AssertFatal ((void)0)
#endif

#endif

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
//*/

#include "stream.h"
#include "FileStream.h"
#include "FilterStream.h"
#include "MemStream.h"
#include "ResizeFilterStream.h"
#include "ZipAggregate.h"
#include "zipHeader.h"
#include "ZipSubStream.h"
#include "file.h"
#include "PackAggregate.h"
#include "PackSubStream.h"
#include "memmapfilestream.h"

#endif