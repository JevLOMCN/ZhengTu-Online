/**
 * \file
 * \version  $Id: zJpeg.h 916  $
 * \author  
 * \date 
 * \brief png格式的验证码生成器
 */


#ifndef _zJpeg_h_
#define _zJpeg_h_

namespace Zebra
{
	void *jpegPassport(char *buffer, const int buffer_len, int *size);
};

#endif

