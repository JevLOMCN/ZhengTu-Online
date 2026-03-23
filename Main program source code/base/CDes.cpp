#include "CDes.h"
#include <strings.h>
#include <iostream>

/*
#undef c2l
#define c2l(c,l) (l =((unsigned long)(*((c)++))), \
		l|=((unsigned long)(*((c)++)))<< 8L, \
		l|=((unsigned long)(*((c)++)))<<16L, \
		l|=((unsigned long)(*((c)++)))<<24L)

#undef l2c
#define l2c(l,c) (*((c)++)=(unsigned char)(((l))&0xff), \
		*((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
		*((c)++)=(unsigned char)(((l)>>16L)&0xff), \
		*((c)++)=(unsigned char)(((l)>>24L)&0xff))
*/

CDes::CDes()
{
	schedule = new DES_key_schedule;
	haveKey = false;
}

CDes::~CDes() 
{
	SAFE_DELETE(schedule);
}

void CDes::setkey(const_DES_cblock *key)
{
	if (0==key) return;
	/*
	SAFE_DELETE(schedule);
	*/
	int ret = DES_set_key_checked(key, schedule); 
	if (0==ret)
		haveKey = true;
	else
		std::cout<<"can not set key! ret = "<<ret<<std::endl;
}

int CDes::encdec(unsigned char *data, unsigned int nLen, bool enc)
{
	if ((0==data)||(!haveKey)) return -1;
	
	unsigned int offset = 0;
	while (offset<=nLen-8)
	{
		DES_cblock tmp;
		DES_ecb_encrypt((DES_cblock *)(data+offset), &tmp, schedule, enc);
		bcopy(&tmp, data+offset, 8);
		//DES_encrypt1((DES_LONG*)(data+offset), schedule, enc);
		offset += 8;
	}

	return nLen-offset;
}
