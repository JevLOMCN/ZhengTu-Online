#ifndef _CDES_H
#define _CDES_H

#include "des.h"

class CDes
{
	public:
		CDes();
		~CDes();

		void setkey(const_DES_cblock *key);
		int encdec(unsigned char *data, unsigned int nLen, bool enc);
	private:
		DES_key_schedule *schedule;
		bool haveKey;
};

#endif
