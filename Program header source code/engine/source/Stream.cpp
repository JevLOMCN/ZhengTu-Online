/********************************************************************
	创建时间:	2004/10/31   14:35
	文件名: 	Stream.cpp
	文件类型:	cpp
	作者:		$$$03公司
	
	用途:	  流基类
*********************************************************************/

// Stream.cpp: implementation of the Stream class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/Stream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Stream::Stream()
 : m_streamStatus(Closed)
{

}

Stream::~Stream()
{
}

const TCHAR* Stream::getStatusString(const Status in_status)
{
	switch (in_status) {
	case Ok:
		return _T("StreamOk");
	case IOError:
		return _T("StreamIOError");
	case EOS:
		return _T("StreamEOS");
	case IllegalCall:
		return _T("StreamIllegalCall");
	case Closed:
		return _T("StreamClosed");
	case UnknownError:
		return _T("StreamUnknownError");
		
	default:
		return _T("Invalid Stream::Status");
	}
}